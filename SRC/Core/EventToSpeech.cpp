// Event to speech.
#include "EventToSpeech.h"
#include <Interfaces/Object.h>
#include "EventHandler.h"
#include "Logger.h"
#include <functional>
#include <cmath>
#include "App.h"
#include "FocusManager.h"
#include "KeyboardHandler.h"
#include <algorithm>
#include <sstream>

/*
This static function attempts to find a named object if the object that received the focus gain event doesn't have a name.
For example, Mate system info has list items, which also contain a bunch of obscure nested elements, and somewhere in there are information labels.
*/
[[nodiscard]] static auto FindAnnouncementInHierarchy(const std::shared_ptr<IObject>& obj, bool recursive = true, bool collect_all_labels = true) -> std::string {
	if (!obj) return "";
	LogCalled();

	auto type = obj->GetType().value_or(IObject::UNKNOWN);
	if (type == IObject::LIST_ITEM || collect_all_labels) {
		std::vector<std::string> texts;

		std::function<void(const std::shared_ptr<IObject>&)> CollectLabels = [&](const std::shared_ptr<IObject>& current) {
			if (!current) return;
			LogCalled();

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

	if (auto text_provider = obj->GetAs<ITextProvider>()) {
		if (auto text = text_provider->GetText(text_provider->GetCursor().value_or(0), ETextGranularity::LINE)) {
			announcement = text->text;
			if (!announcement.empty()) {
				return announcement;
			}
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
[[nodiscard]] static auto FindAnnouncementOfCursorPosition(const std::shared_ptr<ITextProvider>& obj, int previous_cursor_position, ETextGranularity& granularity) -> std::string {
	if (!obj) return "";

	LogCalled();
	auto current_cursor = obj->GetCursor();
	if (!current_cursor) {
		g_logger.Log(CLogger::ERROR, std::string(IObject::ErrorToString(current_cursor.error())));
		return "";
	}

	bool vertical_keys_down = g_keyboardHandler.IsKeyDown(CKeyboardEvent::KEYCODE_UP) || g_keyboardHandler.IsKeyDown(CKeyboardEvent::KEYCODE_DOWN) || 
		g_keyboardHandler.IsKeyDown(CKeyboardEvent::KEYCODE_PAGE_UP) || g_keyboardHandler.IsKeyDown(CKeyboardEvent::KEYCODE_PAGE_DOWN);
	bool horizontal_keys_down = g_keyboardHandler.IsKeyDown(CKeyboardEvent::KEYCODE_RIGHT) || g_keyboardHandler.IsKeyDown(CKeyboardEvent::KEYCODE_LEFT) || 
		g_keyboardHandler.IsKeyDown(CKeyboardEvent::KEYCODE_HOME) || g_keyboardHandler.IsKeyDown(CKeyboardEvent::KEYCODE_END);
	bool control_down = g_keyboardHandler.GetModifiers() & CKeyboardEvent::MODIFIER_CTRL;

	if (vertical_keys_down) granularity = ETextGranularity::LINE;
	else if (horizontal_keys_down) {
		granularity = control_down ? ETextGranularity::WORD : ETextGranularity::CHARACTER;
	}

	else  granularity = ETextGranularity::LINE;

	if (auto keys_range = obj->GetText(current_cursor.value_or(0), granularity)) {
		return keys_range->text;
	}

	return "";
}

/*
This is the final step of object event processing. Announce it.
*/
/*
This function tries to find parents who have not been announced or have changed, and pushes the events without the "now" flag in reverse order, including the last object.
*/
auto CEventToSpeech::AnnounceWhereAmI() -> bool {
	/*
	This will be a common practice when we send artificial events.
	Since the event doesn't post, we don't want to duplicate the focus change announcer code.
	*/
	auto object = g_focusManager.GetFocus();
	if (!object) {
		g_speechEngine.Speak("Unknown area", true);
		return true;
	}

	LogCalled();
	const auto chain = g_focusManager.GetContext();

	size_t diff_index{0};
	size_t min_size = std::min(chain.size(), m_contextChain.size());

	while (diff_index < min_size && chain[diff_index] == m_contextChain[diff_index]) {
		++diff_index;
	}

	if (diff_index == chain.size() && chain.size() == m_contextChain.size()) {
		return false;
	}

	std::string last_name{""};
	for (size_t i = diff_index; i < chain.size(); ++i) {
		auto current_object = chain[i];

		auto it = std::find(m_contextChain.begin(), m_contextChain.end(), current_object);
		if (it != m_contextChain.end()) continue;

		auto current_name = current_object->GetName().value_or("");

		if (current_name.empty() || current_name == last_name) {
			continue;
		}

		last_name = current_name;

		CObjectEvent object_event;
		object_event.object = current_object;
		CEvent to_post(std::move(object_event), CEvent::FOCUS_GAINED, false);
		AnnounceFocusChange(to_post);
	}

	CObjectEvent object_event;
	object_event.object = object;
	CEvent to_post(std::move(object_event), CEvent::FOCUS_GAINED, false);
	AnnounceFocusChange(to_post);

	m_contextChain = chain;
	return true;
}

// Various Announcers
void CEventToSpeech::AnnounceFocusChange(CEvent& event) {
	if (event.GetNow() && AnnounceWhereAmI()) {
		// We are canceling this event, since AnnounceWhereAmI is announcing the last object, but without the "now" flag.
		return;
	}

	auto object_event = event.GetAs<CObjectEvent>();
	if (!object_event.has_value()) {
		g_logger.Log(CLogger::ERROR, "Announcer", "Bad access to object event");
		return;
	}

	// g_logger.Log(CLogger::DEBUG, "Focus", DumpObjectToString(object_event.value().object, 0, true));

	LogCalled();

	std::string announcement = FindAnnouncementInHierarchy(object_event.value().object);
	auto type = object_event.value().object->GetType().value_or(IObject::UNKNOWN);
	auto state = object_event.value().object->GetState().value_or(IObject::NO);

	announcement += cSeparator + IObject::GetTypeName(type);

	auto& settings = g_applicationInstance.GetSettings();
	switch (type) {
		case IObject::MENU_ITEM:
		case IObject::LIST_ITEM: {
			if (!settings.read_list_item_count) break;
			auto index = object_event.value().object->GetIndex().value_or(0) + 1;
			auto parent = object_event.value().object->GetParent();
			if (!parent) break;
			auto locked_parent = parent->lock();
			if (!locked_parent) break;
			auto children_count = locked_parent->GetChildrenCount().value_or(0);
			announcement += cSeparator + std::to_string(index) + " of " + std::to_string(children_count);
			break;
		}
		default: break;
	}

	auto state_names = IObject::GetStateNames(type, state);
	for (std::string& state_name : state_names) {
		announcement += cSeparator + state_name;
	}

	g_speechEngine.Speak(std::string_view(announcement), event.GetNow());
	if (auto description = object_event.value().object->GetDescription()) {
		if (!description->empty()) {
			g_speechEngine.Speak(*description, false);
		}
	}

	switch (type) {
		case IObject::SLIDER: {
			CObjectEvent object_event_to_post;
			object_event_to_post.object = object_event.value().object;
			CEvent to_post(std::move(object_event_to_post), CEvent::VALUE_CHANGED, false);
			AnnounceValueChange(to_post);
			break;
		}
		case IObject::TEXT_FIELD: {
			CObjectEvent object_event_to_post;
			object_event_to_post.object = object_event.value().object;
			CEvent to_post(std::move(object_event_to_post), CEvent::CURSOR_MOVED, false);
			AnnounceCursorMove(to_post);
			break;
		}
		default: break;
	}
}

void CEventToSpeech::AnnounceValueChange(CEvent& event) {
	auto object_event = event.GetAs<CObjectEvent>();
	if (!object_event.has_value()) {
		g_logger.Log(CLogger::ERROR, "Announcer", "Bad access to object event");
		return;
	}
	LogCalled();

	if (object_event.value().object->GetType () != IObject::SLIDER || g_focusManager.GetFocus() != object_event.value().object) return;

	if (auto value_provider = object_event.value().object->GetAs<IValueProvider>()) {
		std::ostringstream oss;
		oss << value_provider->GetCurrentValue().value_or(0);
		g_speechEngine.Speak(std::string_view(oss.str()), event.GetNow());
	}
}

void CEventToSpeech::AnnounceStateChange(CEvent& event) {
	auto object_event = event.GetAs<CObjectEvent>();
	if (!object_event.has_value()) {
		g_logger.Log(CLogger::ERROR, "Announcer", "Bad access to object event");
		return;
	}

	LogCalled();
	if (g_focusManager.GetFocus() != object_event.value().object) return;
	std::string announcement = "";
	auto type = object_event.value().object->GetType().value_or(IObject::UNKNOWN);
	auto state = object_event.value().object->GetState().value_or(IObject::NO);

	auto state_names = IObject::GetStateNames(type, state);
	for (std::string& state_name : state_names) {
		announcement += cSeparator + state_name;
	}

	g_speechEngine.Speak(std::string_view(announcement), event.GetNow());
}

void CEventToSpeech::AnnounceSelectionChange(CEvent& event) {
	auto object_event = event.GetAs<CObjectEvent>();
	if (!object_event.has_value()) {
		g_logger.Log(CLogger::ERROR, "Announcer", "Bad access to object event");
		return;
	}

	LogCalled();

	if (auto text_provider = object_event.value().object->GetAs<ITextProvider>()) {
		if (auto text_selections = text_provider->GetSelectedRanges()) {
			for (const auto& text_selection : *text_selections) {
				g_speechEngine.Speak(text_selection.text, false);
			}

			if (!text_selections->empty()) g_speechEngine.Speak("selected", false);
		}
	}
}

void CEventToSpeech::AnnounceCursorMove(CEvent& event) {
	auto object_event = event.GetAs<CObjectEvent>();
	if (!object_event.has_value()) {
		g_logger.Log(CLogger::ERROR, "Announcer", "Bad access to object event");
		return;
	}

	LogCalled();
	//if (g_focusManager.GetFocus() != object_event.value().object) return;

	if (auto text_provider = object_event.value().object->GetAs<ITextProvider>()) {
		auto cursor = text_provider->GetCursor();
		if (!cursor) {
			return; 
		}

		ETextGranularity granularity{ETextGranularity::CHARACTER};
		std::string announcement = FindAnnouncementOfCursorPosition(text_provider, 0, granularity);
		bool enable_spelling{false};
		g_speechSystem.SetParameter(g_speechEngineIndex, SRAL_PARAM_ENABLE_SPELLING, granularity == ETextGranularity::CHARACTER ? true : false);
		g_speechEngine.Speak(std::string_view(announcement), event.GetNow());
		g_speechSystem.SetParameter(g_speechEngineIndex, SRAL_PARAM_ENABLE_SPELLING, false);
	}
}
