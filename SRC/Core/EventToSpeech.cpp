// Event to speech.
module;
#include "Logger.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <sstream>
module Core.EventToSpeech;
import Core.App;
import Core.Event;
import Core.KeyboardHandler;
import Core.Object;
import Core.Text;
import Proxies.Object;
import Traits.SpeechEngine;

/*
This static function attempts to find a named object if the object that received the focus gain event doesn't have a
name. For example, Mate system info has list items, which also contain a bunch of obscure nested elements, and somewhere
in there are information labels.
*/
static void FindAnnouncementInHierarchy(
	std::pmr::string& out, CObjectProxy obj, bool recursive = true, bool collect_all_labels = true) {
	if (!obj.IsValid())
		return;

	auto collect_labels_recursive = [&](auto& self, auto&& current) -> void {
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

		if (auto children_count = current.GetChildrenCount()) {
			for (auto i = 0; i < *children_count; ++i) {
				auto child = current.GetChildAt(i);
				if (child)
					self(self, *child);
			}
		}
	};

	auto type = obj.GetType().value_or(EObjectType::UNKNOWN);

	if ((!IsObjectContainer(type) && !IsObjectParent(type)) && collect_all_labels) {
		collect_labels_recursive(collect_labels_recursive, obj);
		if (!out.empty())
			return;
	}

	std::string announcement = obj.GetName().value_or("");
	if (!announcement.empty()) {
		out += announcement;
		return;
	}
	auto text_provider = obj.GetAs<CTextProviderProxy>();
	if (auto text = text_provider.GetText(text_provider.GetCursor().value_or(0), ETextGranularity::LINE)) {
		announcement = text->text;
		if (!announcement.empty()) {
			out += announcement;
		}
	}

	if (recursive) {
		if (auto children_count = obj.GetChildrenCount()) {
			for (auto i = 0; i < *children_count; ++i) {
				if (auto child = obj.GetChildAt(i))
					FindAnnouncementInHierarchy(out, *child, true, collect_all_labels);
			}
		}
	}
}

/*
This static function tries to determine where the cursor has moved and returns a chunk of text.
Granularity is needed if the cursor has moved one character, in which case spelling should be enabled.
*/
static void FindAnnouncementOfCursorPosition(
	std::pmr::string& out, CTextProviderProxy provider, ETextGranularity& granularity) {
	LogCalled();

	auto current_cursor = provider.GetCursor();
	if (!current_cursor) {
		g_logger.Log(CLogger::ERROR, std::string(ObjectErrorToString(current_cursor.error())));
		return;
	}

	auto& keyboard_handler = CKeyboardHandler::GetInstance();
	bool vertical_keys_down = keyboard_handler.IsKeyDown(CKeyboardEvent::KEYCODE_UP) ||
		keyboard_handler.IsKeyDown(CKeyboardEvent::KEYCODE_DOWN) ||
		keyboard_handler.IsKeyDown(CKeyboardEvent::KEYCODE_PAGE_UP) ||
		keyboard_handler.IsKeyDown(CKeyboardEvent::KEYCODE_PAGE_DOWN);
	bool horizontal_keys_down = keyboard_handler.IsKeyDown(CKeyboardEvent::KEYCODE_RIGHT) ||
		keyboard_handler.IsKeyDown(CKeyboardEvent::KEYCODE_LEFT) ||
		keyboard_handler.IsKeyDown(CKeyboardEvent::KEYCODE_HOME) ||
		keyboard_handler.IsKeyDown(CKeyboardEvent::KEYCODE_END);
	bool control_down = keyboard_handler.GetModifiers() & CKeyboardEvent::MODIFIER_CTRL;

	if (vertical_keys_down)
		granularity = ETextGranularity::LINE;
	else if (horizontal_keys_down) {
		granularity = control_down ? ETextGranularity::WORD : ETextGranularity::CHARACTER;
	}

	else
		granularity = ETextGranularity::LINE;

	if (auto keys_range = provider.GetText(current_cursor.value_or(0), granularity)) {
		out += keys_range->text;
	}
}

// Announcement builders
void CEventToSpeech::BuildFocusAnnouncement(std::pmr::string& out, CObjectProxy obj, bool require_all) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	FindAnnouncementInHierarchy(out, obj, !m_isWhereAmIOperation, !m_isWhereAmIOperation);
	auto type = obj.GetType().value_or(EObjectType::UNKNOWN);
	if (!out.empty())
		Separate(out);
	out += GetObjectTypeName(type, out.empty() ? true : require_all);
	Separate(out);
	BuildStateAnnouncement(out, obj, require_all);

	auto& settings = CScreenReaderApp::GetInstance().GetSettings();
	if (settings.read_list_item_count && IsObjectDataElement(type)) {
		auto index = obj.GetIndex().value_or(0) + 1;
		auto parent = obj.GetParent();
		if (parent || parent->IsValid()) {
			auto children_count = parent->GetChildrenCount().value_or(0);
			Separate(out);
			std::format_to(std::back_inserter(out), "{}{} of {}", cSeparator, index, children_count);
		}
	}
	Separate(out);
	BuildValueAnnouncement(out, obj);
	Separate(out);
	BuildTextAnnouncement(out, obj);

	if (auto description = obj.GetDescription()) {
		if (!description->empty() && !out.starts_with(*description)) {
			Separate(out);
			out += *description;
		}
	}
}

void CEventToSpeech::BuildStateAnnouncement(std::pmr::string& out, CObjectProxy obj, bool require_all) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	auto type = obj.GetType();
	if (!type)
		return;

	if (auto state = obj.GetState()) {
		auto state_names = GetObjectStateNames(*type, *state);
		for (std::string_view state_name : state_names) {
			std::format_to(std::back_inserter(out), "{}{}", cSeparator, state_name);
		}
	}
}

void CEventToSpeech::BuildValueAnnouncement(std::pmr::string& out, CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	auto value_provider = obj.GetAs<CValueProviderProxy>();
	if (auto current = value_provider.GetCurrent()) {
		std::format_to(std::back_inserter(out), "{}", *current);
	}
}

void CEventToSpeech::BuildTextAnnouncement(std::pmr::string& out, CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	auto text_provider = obj.GetAs<CTextProviderProxy>();
	if (auto text = text_provider.GetText(text_provider.GetCursor().value_or(0), ETextGranularity::LINE)) {
		out += text->text;
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
	auto object = m_focusManager.GetFocus();
	if (!object.IsValid()) {
		m_speechSystem.Speak("Unknown area");
		return true;
	}

	LogCalled();
	const auto chain = m_focusManager.GetContext();

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

	LogCalled();

	std::pmr::string announcement(&m_pool);
	BuildFocusAnnouncement(announcement, object_event.value().object);
	m_speechSystem.Speak(announcement, event.GetNow());
}

void CEventToSpeech::AnnounceValueChange(CEvent& event) {
	auto object_event = event.GetAs<CObjectEvent>();
	if (!object_event.has_value()) {
		g_logger.Log(CLogger::ERROR, "Announcer", "Bad access to object event");
		return;
	}

	auto type = object_event.value().object.GetType();
	if (!type || !IsObjectValue(*type))
		return;
	std::pmr::string announcement(&m_pool);
	BuildValueAnnouncement(announcement, object_event.value().object);
	m_speechSystem.Speak(announcement, event.GetNow());
}

void CEventToSpeech::AnnounceStateChange(CEvent& event) {
	auto object_event = event.GetAs<CObjectEvent>();
	if (!object_event.has_value()) {
		g_logger.Log(CLogger::ERROR, "Announcer", "Bad access to object event");
		return;
	}

	LogCalled();
	std::pmr::string announcement(&m_pool);
	BuildStateAnnouncement(announcement, object_event.value().object);
	m_speechSystem.Speak(announcement, event.GetNow());
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

	auto text_provider = object_event.value().object.GetAs<CTextProviderProxy>();
	auto cursor = text_provider.GetCursor();
	if (!cursor) {
		return;
	}

	ETextGranularity granularity{ETextGranularity::CHARACTER};
	std::pmr::string announcement(&m_pool);
	FindAnnouncementOfCursorPosition(announcement, text_provider, granularity);
	if (granularity == ETextGranularity::CHARACTER)
		m_speechSystem.Spell(std::string_view(announcement), event.GetNow());
	else
		m_speechSystem.Speak(std::string_view(announcement), event.GetNow());
}
