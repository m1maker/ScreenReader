// Event to speech.
#include "EventToSpeech.h"

#include "App.h"
#include "EventHandler.h"
#include "FocusManager.h"
#include "KeyboardHandler.h"
#include "Logger.h"
#include "Utf8.h"

#include <Core/ScopedPool.h>
#include <algorithm>
#include <cmath>
#include <functional>
#include <sstream>

/*
This static function attempts to find a named object if the object that received the focus gain event doesn't have a
name. For example, Mate system info has list items, which also contain a bunch of obscure nested elements, and somewhere
in there are information labels.
*/
static void FindAnnouncementInHierarchy(
	std::pmr::string& out, CObject obj, bool recursive = true, bool collect_all_labels = true) {
	if (!obj.IsValid())
		return;

	DefaultPool(pool);

	auto collect_labels_recursive = [&](auto& self, CObject current) -> void {
		if (!current.IsValid())
			return;

		auto current_type = current.GetType().value_or(EObjectType::UNKNOWN);
		if (current_type == EObjectType::LABEL) {
			auto name = current.GetName().value_or("");
			if (!name.empty()) {
				if (!out.empty())
					out += CEventToSpeech::cSeparator;
				out += name;
			}
		}

		if (auto children_res = current.GetChildren()) {
			for (const auto child : *children_res) {
				self(self, child);
			}
		}
	};

	auto type = obj.GetType().value_or(EObjectType::UNKNOWN);

	if ((type == EObjectType::LIST_ITEM || type == EObjectType::MENU_ITEM) && collect_all_labels) {
		collect_labels_recursive(collect_labels_recursive, obj);
		if (!out.empty())
			return;
	}

	std::string announcement = obj.GetName().value_or("");
	if (!announcement.empty()) {
		out += announcement;
		return;
	}

	if (auto text = obj.GetText(obj.GetCursor().value_or(0), ETextGranularity::LINE)) {
		announcement = text->text;
		if (!announcement.empty()) {
			out += announcement;
		}
	}

	if (recursive) {
		if (auto children = obj.GetChildren()) {
			for (const auto child : *children) {
				FindAnnouncementInHierarchy(out, child, true, collect_all_labels);
			}
		}
	}
}

/*
This static function tries to determine where the cursor has moved and returns a chunk of text.
Granularity is needed if the cursor has moved one character, in which case spelling should be enabled.
*/
static void FindAnnouncementOfCursorPosition(std::pmr::string& out, CObject obj, ETextGranularity granularity) {
	if (!obj.IsValid())
		return;

	LogCalled();

	auto current_cursor = obj.GetCursor();
	if (!current_cursor) {
		g_logger.Log(CLogger::ERROR, std::string(ObjectErrorToString(current_cursor.error())));
		return;
	}

	bool vertical_keys_down = g_keyboardHandler.IsKeyDown(CKeyboardEvent::KEYCODE_UP) ||
		g_keyboardHandler.IsKeyDown(CKeyboardEvent::KEYCODE_DOWN) ||
		g_keyboardHandler.IsKeyDown(CKeyboardEvent::KEYCODE_PAGE_UP) ||
		g_keyboardHandler.IsKeyDown(CKeyboardEvent::KEYCODE_PAGE_DOWN);
	bool horizontal_keys_down = g_keyboardHandler.IsKeyDown(CKeyboardEvent::KEYCODE_RIGHT) ||
		g_keyboardHandler.IsKeyDown(CKeyboardEvent::KEYCODE_LEFT) ||
		g_keyboardHandler.IsKeyDown(CKeyboardEvent::KEYCODE_HOME) ||
		g_keyboardHandler.IsKeyDown(CKeyboardEvent::KEYCODE_END);
	bool control_down = g_keyboardHandler.GetModifiers() & CKeyboardEvent::MODIFIER_CTRL;

	if (vertical_keys_down)
		granularity = ETextGranularity::LINE;
	else if (horizontal_keys_down) {
		granularity = control_down ? ETextGranularity::WORD : ETextGranularity::CHARACTER;
	}

	else
		granularity = ETextGranularity::LINE;

	if (auto keys_range = obj.GetText(current_cursor.value_or(0), granularity)) {
		out += keys_range->text;
	}
}

/*
This is the final step of object event processing. Announce it.
*/
/*
This function tries to find parents who have not been announced or have changed, and pushes the events without the "now"
flag in reverse order, including the last object.
*/
auto CEventToSpeech::AnnounceWhereAmI() -> bool {
	/*
	This will be a common practice when we send artificial events.
	Since the event doesn't post, we don't want to duplicate the focus change announcer code.
	*/
	auto object = g_focusManager.GetFocus();
	if (!object.IsValid()) {
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
	m_isWhereAmIOperation = true;
	for (size_t i = diff_index; i < chain.size(); ++i) {
		const auto& current_object = chain[i];

		auto it = std::ranges::find(m_contextChain, current_object);
		if (it != m_contextChain.end())
			continue;

		auto current_name = current_object.GetName().value_or("");

		if (current_name.empty() || current_name == last_name) {
			continue;
		}

		last_name = current_name;

		CObjectEvent object_event;
		object_event.object = current_object;
		object_event.type = EObjectEventType::FOCUS_GAINED;
		CEvent to_post(std::move(object_event), false);
		AnnounceFocusChange(to_post);
	}
	m_isWhereAmIOperation = false;
	CObjectEvent object_event;
	object_event.object = object;
	object_event.type = EObjectEventType::FOCUS_GAINED;
	CEvent to_post(std::move(object_event), false);
	AnnounceFocusChange(to_post);

	m_contextChain = chain;
	return true;
}

// Various Announcers
void CEventToSpeech::AnnounceFocusChange(CEvent& event) {
	if (event.GetNow() && AnnounceWhereAmI()) {
		// We are canceling this event, since AnnounceWhereAmI is announcing the last object, but without the "now"
		// flag.
		return;
	}

	auto object_event = event.GetAs<CObjectEvent>();
	if (!object_event.has_value()) {
		g_logger.Log(CLogger::ERROR, "Announcer", "Bad access to object event");
		return;
	}

	// g_logger.Log(CLogger::DEBUG, "Focus", DumpObjectToString(object_event.value().object, 0, true));

	DefaultPool(pool);
	LogCalled();

	std::pmr::string announcement(&pool);
	FindAnnouncementInHierarchy(
		announcement, object_event.value().object, !m_isWhereAmIOperation, !m_isWhereAmIOperation);
	auto type = object_event.value().object.GetType().value_or(EObjectType::UNKNOWN);
	auto state = object_event.value().object.GetState().value_or(0);

	announcement += cSeparator;
	announcement += GetObjectTypeName(type);

	auto& settings = g_applicationInstance.GetSettings();
	switch (type) {
	case EObjectType::MENU_ITEM:
	case EObjectType::LIST_ITEM: {
		if (!settings.read_list_item_count)
			break;
		auto index = object_event.value().object.GetIndex().value_or(0) + 1;
		auto parent = object_event.value().object.GetParent().value_or(CObject());
		if (!parent.IsValid())
			break;
		auto children_count = parent.GetChildrenCount().value_or(0);
		std::format_to(std::back_inserter(announcement), "{}{} of {}", cSeparator, index, children_count);
		break;
	}
	default:
		break;
	}

	auto state_names = GetObjectStateNames(type, state);
	for (std::string_view state_name : state_names) {
		std::format_to(std::back_inserter(announcement), "{}{}", cSeparator, state_name);
	}

	g_speechEngine.Speak(announcement, event.GetNow());
	if (auto description = object_event.value().object.GetDescription()) {
		if (!description->empty()) {
			g_speechEngine.Speak(*description, false);
		}
	}

	switch (type) {
	case EObjectType::SLIDER: {
		CObjectEvent object_event_to_post;
		object_event_to_post.object = object_event.value().object;
		object_event_to_post.type = EObjectEventType::VALUE_CHANGED;
		CEvent to_post(std::move(object_event_to_post), false);
		AnnounceValueChange(to_post);
		break;
	}
	case EObjectType::TEXT_FIELD: {
		CObjectEvent object_event_to_post;
		object_event_to_post.object = object_event.value().object;
		object_event_to_post.type = EObjectEventType::CURSOR_MOVED;
		CEvent to_post(std::move(object_event_to_post), false);
		AnnounceCursorMove(to_post);
		break;
	}
	default:
		break;
	}
}

void CEventToSpeech::AnnounceValueChange(CEvent& event) {
	auto object_event = event.GetAs<CObjectEvent>();
	if (!object_event.has_value()) {
		g_logger.Log(CLogger::ERROR, "Announcer", "Bad access to object event");
		return;
	}
	LogCalled();

	if (object_event.value().object.GetType() != EObjectType::SLIDER ||
		g_focusManager.GetFocus() != object_event.value().object)
		return;

	ScopedPool(pool, 256);
	std::pmr::string announcement(&pool);
	std::format_to(std::back_inserter(announcement), "{}", object_event.value().object.GetCurrentValue().value_or(0));
	g_speechEngine.Speak(std::string_view(announcement), event.GetNow());
}

void CEventToSpeech::AnnounceStateChange(CEvent& event) {
	auto object_event = event.GetAs<CObjectEvent>();
	if (!object_event.has_value()) {
		g_logger.Log(CLogger::ERROR, "Announcer", "Bad access to object event");
		return;
	}

	LogCalled();
	if (g_focusManager.GetFocus() != object_event.value().object)
		return;

	DefaultPool(pool);

	std::pmr::string announcement(&pool);
	auto type = object_event.value().object.GetType().value_or(EObjectType::UNKNOWN);
	auto state = object_event.value().object.GetState().value_or(0);

	auto state_names = GetObjectStateNames(type, state);
	for (auto state_name : state_names) {
		std::format_to(std::back_inserter(announcement), "{}{}", cSeparator, state_name);
	}

	g_speechEngine.Speak(announcement, event.GetNow());
}

void CEventToSpeech::AnnounceSelectionChange(CEvent& event) {
	auto object_event = event.GetAs<CObjectEvent>();
	if (!object_event.has_value()) {
		g_logger.Log(CLogger::ERROR, "Announcer", "Bad access to object event");
		return;
	}

	LogCalled();
}

void CEventToSpeech::AnnounceCursorMove(CEvent& event) {
	auto object_event = event.GetAs<CObjectEvent>();
	if (!object_event.has_value()) {
		g_logger.Log(CLogger::ERROR, "Announcer", "Bad access to object event");
		return;
	}

	LogCalled();
	// if (g_focusManager.GetFocus() != object_event.value().object) return;

	auto cursor = object_event.value().object.GetCursor();
	if (!cursor) {
		return;
	}

	DefaultPool(pool);
	ETextGranularity granularity{ETextGranularity::CHARACTER};
	std::pmr::string announcement(&pool);
	FindAnnouncementOfCursorPosition(announcement, object_event.value().object, granularity);
	bool enable_spelling{false};
	g_speechSystem.SetParameter(
		g_speechEngineIndex, SRAL_PARAM_ENABLE_SPELLING, granularity == ETextGranularity::CHARACTER ? true : false);
	g_speechEngine.Speak(std::string_view(announcement), event.GetNow());
	g_speechSystem.SetParameter(g_speechEngineIndex, SRAL_PARAM_ENABLE_SPELLING, false);
}
