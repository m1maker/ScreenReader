// Event to speech.
#include "EventToSpeech.h"
#include "Object.h"
#include "EventHandler.h"
#include "Logger.h"
#include <functional>
#include <cmath>
#include "App.h"
#include "FocusManager.h"
#include "KeyboardHandler.h"

/*
This static function attempts to find a named object if the object that received the focus gain event doesn't have a name.
For example, Mate system info has list items, which also contain a bunch of obscure nested elements, and somewhere in there are information labels.
*/
[[nodiscard]] static auto FindAnnouncementInHierarchy(const std::shared_ptr<IObject>& obj, bool recursive = true, bool collect_all_labels = true) -> std::string {
	if (!obj) return "";

	auto type = obj->GetType().value_or(IObject::UNKNOWN);
	if (type == IObject::LIST_ITEM || collect_all_labels) {
		std::vector<std::string> texts;

		std::function<void(const std::shared_ptr<IObject>&)> CollectLabels = [&](const std::shared_ptr<IObject>& current) {
			if (!current) return;

			auto current_type = obj->GetType().value_or(IObject::UNKNOWN);
			if (current_type == IObject::LABEL) {
				std::string name = current->GetName().value_or("");
				if (!name.empty()) {
					texts.push_back(name);
				}
			}

			if (auto children = current->GetChildren()) {
				for (const auto& child : *children) {
					CollectLabels(child);
				}
			}
		};

		CollectLabels(obj);

		if (!texts.empty()) {
			std::string result;
			for (size_t i = 0; i < texts.size(); ++i) {
				if (i > 0) {
					result += CEventToSpeech::cSeparator;
				}
				result += texts[i];
			}
			return result;
		}
	}

	std::string announcement = obj->GetName().value_or("");
	if (!announcement.empty()) {
		return announcement;
	}

	if (auto text = obj->GetText(obj->GetCursor().value_or(0), ETextGranularity::LINE)) {
		announcement = text->text;
		if (!announcement.empty()) {
			return announcement;
		}
	}

	if (recursive) {
		if (auto children = obj->GetChildren()) {
			for (const auto& child : *children) {
				std::string child_announcement = FindAnnouncementInHierarchy(child, true, collect_all_labels);
				if (!child_announcement.empty()) {
					return child_announcement;
				}
			}
		}
	}

	return "";
}

/*
This static function tries to determine where the cursor has moved and returns a chunk of text.
Granularity is needed if the cursor has moved one character, in which case spelling should be enabled.
*/
[[nodiscard]] static auto FindAnnouncementOfCursorPosition(const std::shared_ptr<IObject>& obj, int previous_cursor_position, ETextGranularity& granularity) -> std::string {
	if (!obj) return "";

	int current_cursor = obj->GetCursor().value();
	int delta = std::abs(current_cursor - previous_cursor_position);

	bool vertical_keys_down = g_keyboardHandler.IsKeyDown(CKeyboardEvent::KEYCODE_UP) || g_keyboardHandler.IsKeyDown(CKeyboardEvent::KEYCODE_DOWN);
	bool horizontal_keys_down = g_keyboardHandler.IsKeyDown(CKeyboardEvent::KEYCODE_RIGHT) || g_keyboardHandler.IsKeyDown(CKeyboardEvent::KEYCODE_LEFT);
	bool control_down = g_keyboardHandler.GetModifiers() & CKeyboardEvent::MODIFIER_CTRL;

	granularity = ETextGranularity::LINE;
	STextRange line_range = obj->GetText(current_cursor, granularity).value();

	if (vertical_keys_down || previous_cursor_position < line_range.start || previous_cursor_position >= line_range.end) {
		return line_range.text;
	}

	if (horizontal_keys_down || delta == 1) {
		granularity = ETextGranularity::CHARACTER;
		STextRange char_range = obj->GetText(current_cursor, granularity).value();
		return char_range.text;
	}
	granularity = ETextGranularity::WORD;
	STextRange word_range = obj->GetText(current_cursor, granularity).value();
	if ((control_down && horizontal_keys_down) || previous_cursor_position < word_range.start || previous_cursor_position >= word_range.end) {
		return word_range.text;
	}
	granularity = ETextGranularity::CHARACTER;
	STextRange char_range = obj->GetText(current_cursor, granularity).value();
	return char_range.text;
}

/*
This is the final step of object event processing. Announce it.
*/
void CEventToSpeech::AnnounceWhereAmI() {
	/*
	This will be a common practice when we send artificial events.
	Since the event doesn't post, we don't want to duplicate the focus change announcer code.
	*/
	auto object = g_focusManager.GetFocus();
	if (!object) {
		g_speechEngine.Speak("Unknown area", true);
		return;
	}

	const auto& chain = g_focusManager.GetContext();
	for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
		CObjectEvent object_event;
		object_event.object = *it;
		CEvent to_post(object_event, CEvent::FOCUS_GAINED, false);
		AnnounceFocusChange(to_post);
	}
}

// Various Announcers
void CEventToSpeech::AnnounceFocusChange(CEvent& event) {
	auto object_event = event.GetAs<CObjectEvent>();
	if (!object_event.has_value()) {
		g_logger.Log(CLogger::ERROR, "Announcer", "Bad access to object event");
		return;
	}

	g_logger.Log(CLogger::DEBUG, "Focus", DumpObjectToString(object_event.value().object, 0, true));

	/*
	So, the parent updated event is considered focus changed, but it should not be interrupted by subsequent focus gained events.
	*/
	if (event.GetType() == CEvent::PARENT_UPDATED) {
		m_parentAnnounced = true;
		/*
		When processing the "parent updated" event, AT-SPI can, for some reason, send an event with "parent updated" but no associated object.
		Often, these are menu items and the like.
		Let's try to catch those objects that, in 99% of cases, won't be parents of any other elements.
		*/
		if (!IObject::IsValidParent(object_event.value().object->GetType().value())) {
			//m_parentAnnounced = false;
			return;
		}
	}

	std::string announcement = FindAnnouncementInHierarchy(object_event.value().object);
	/*
	I don't know what to do here yet, but I hope I can eventually standardize this behavior.
	When I'm in Caja Explorer, the file/folder name isn't announced in a tree view.
	When I look at it with Orca's object navigator, I see that the first child is blank, and to the right is the file name and other data, such as size, etc.
	Then it says it's the name column header, and then the name again, and so on.
	Now we'll try to find a nearby object that has a name.
	*/
	if (announcement.empty()) {
		auto children = object_event.value().object->GetChildren().value();
		for (const auto& child : children) {
			if (!child->GetName()->empty()) {
				CObjectEvent object_event_to_announce;
				object_event_to_announce.object = child;

				CEvent to_announce(std::move(object_event_to_announce), event.GetType(), event.GetNow());
				AnnounceFocusChange(to_announce);
				return;
			}
		}
	}

	auto type = object_event.value().object->GetType().value();
	announcement += cSeparator + IObject::GetTypeName(type);

	auto& settings = g_applicationInstance.GetSettings();
	switch (type) {
		case IObject::SLIDER:
			announcement += cSeparator + std::to_string(object_event.value().object->GetCurrentValue().value());
			break;
		case IObject::TEXT_FIELD:
			announcement += cSeparator + object_event.value().object->GetText(object_event.value().object->GetCursor().value(), ETextGranularity::LINE)->text;
			break;
		case IObject::MENU_ITEM:
		case IObject::LIST_ITEM: {
			if (!settings.read_list_item_count) break;
			auto index = *object_event.value().object->GetIndex() + 1;
			auto parent = object_event.value().object->GetParent()->lock();
			if (!parent) break;
			auto children_count = *parent->GetChildrenCount();
			announcement += cSeparator + std::to_string(index) + " of " + std::to_string(children_count);
			break;
		}
		default: break;
	}

	auto state_names = IObject::GetStateNames(object_event.value().object->GetType().value(), object_event.value().object->GetState().value());
	for (std::string& state_name : state_names) {
		announcement += cSeparator + state_name;
	}

	/*
	Now we determine whether to ignore the "now" flag.
	If focus has been gained after the "parent updated" event, then we never interrupt the speech.
	Also, all subsequent children up to the final one should not be interrupted, but I can't do this now.
	*/
	g_speechEngine.Speak(std::string_view(announcement),(event.GetType() == CEvent::FOCUS_GAINED && m_parentAnnounced) || event.GetType() == CEvent::PARENT_UPDATED ? false : event.GetNow());
	g_speechEngine.Speak(object_event.value().object->GetDescription().value(), false);

	if (event.GetType() != CEvent::PARENT_UPDATED) m_parentAnnounced = false;
}

void CEventToSpeech::AnnounceValueChange(CEvent& event) {
	auto object_event = event.GetAs<CObjectEvent>();
	if (!object_event.has_value()) {
		g_logger.Log(CLogger::ERROR, "Announcer", "Bad access to object event");
		return;
	}

	if (object_event.value().object->GetType () != IObject::SLIDER || g_focusManager.GetFocus() != object_event.value().object) return;
	g_speechEngine.Speak(std::string_view(std::to_string(object_event.value().object->GetCurrentValue().value())), event.GetNow());
}

void CEventToSpeech::AnnounceStateChange(CEvent& event) {
	auto object_event = event.GetAs<CObjectEvent>();
	if (!object_event.has_value()) {
		g_logger.Log(CLogger::ERROR, "Announcer", "Bad access to object event");
		return;
	}

	if (g_focusManager.GetFocus() != object_event.value().object) return;
	std::string announcement = "";
	auto state_names = IObject::GetStateNames(object_event.value().object->GetType().value(), object_event.value().object->GetState().value());
	for (std::string& state_name : state_names) {
		announcement += cSeparator + state_name;
	}

	g_speechEngine.Speak(std::string_view(announcement), event.GetNow());
}

void CEventToSpeech::AnnounceSelectionChange(CEvent& event) {
	return;
}

void CEventToSpeech::AnnounceCursorMove(CEvent& event) {
	auto object_event = event.GetAs<CObjectEvent>();
	if (!object_event.has_value()) {
		g_logger.Log(CLogger::ERROR, "Announcer", "Bad access to object event");
		return;
	}

	//if (g_focusManager.GetFocus() != object_event.value().object) return;
	ETextGranularity granularity{ETextGranularity::CHARACTER};
	std::string announcement = FindAnnouncementOfCursorPosition(object_event.value().object, object_event.value().previous_cursor_position, granularity);
	bool enable_spelling{false};
	g_speechSystem.SetParameter(g_speechEngineIndex, SRAL_PARAM_ENABLE_SPELLING, granularity == ETextGranularity::CHARACTER ? true : false);
	g_speechEngine.Speak(std::string_view(announcement), true);
	g_speechSystem.SetParameter(g_speechEngineIndex, SRAL_PARAM_ENABLE_SPELLING, false);
}
