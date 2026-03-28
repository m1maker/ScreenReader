module;
#include <cmath>
#include <sstream>
module Core.MessageBuilder;
import Core.App;
import Core.Event;
import Core.KeyboardHandler;
import Core.KeyInfo;
import Core.Object;
import Core.Text;
import Proxies.Object;
import Traits.SpeechEngine;

/*
This static function attempts to find a named object if the object that received the focus gain event doesn't have a
name. For example, Mate system info has list items, which also contain a bunch of obscure nested elements, and somewhere
in there are information labels.
*/
void MessageBuilder::FindAnnouncementInHierarchy(
	std::pmr::string& out, CObjectProxy obj, bool recursive, bool collect_all_labels) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	if (auto name = obj.GetName()) {
		if (!name->empty()) {
			out += *name;
			return;
		}
	}

	auto text_provider = obj.GetAs<CTextProviderProxy>();
	if (auto text = text_provider.GetText(text_provider.GetCursor().value_or(0), ETextGranularity::LINE)) {
		if (!text->text.empty()) {
			out += text->text;
		}
	}

	auto collect_labels_recursive = [&](auto& self, auto&& current) -> void {
		if (!current.IsValid())
			return;

		auto current_type = current.GetType().value_or(EObjectType::UNKNOWN);
		if (current_type == EObjectType::LABEL) {
			auto name = current.GetName().value_or("");
			if (!name.empty()) {
				if (!out.empty())
					Separate(out);
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
void MessageBuilder::FindAnnouncementOfCursorPosition(
	std::pmr::string& out, CTextProviderProxy provider, ETextGranularity& granularity) {
	if (!provider.IsValid()) [[unlikely]]
		return;

	auto current_cursor = provider.GetCursor();
	if (!current_cursor) {
		return;
	}

	auto& keyboard_handler = KeyboardHandler::GetInstance();
	bool vertical_keys_down = keyboard_handler.IsKeyDown(KEYCODE_UP) || keyboard_handler.IsKeyDown(KEYCODE_DOWN) ||
		keyboard_handler.IsKeyDown(KEYCODE_PAGE_UP) || keyboard_handler.IsKeyDown(KEYCODE_PAGE_DOWN);
	bool horizontal_keys_down = keyboard_handler.IsKeyDown(KEYCODE_RIGHT) || keyboard_handler.IsKeyDown(KEYCODE_LEFT) ||
		keyboard_handler.IsKeyDown(KEYCODE_HOME) || keyboard_handler.IsKeyDown(KEYCODE_END);
	bool control_down = keyboard_handler.GetModifiers() & MODIFIER_CTRL;

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
void MessageBuilder::BuildFocusAnnouncement(std::pmr::string& out, CObjectProxy obj, bool require_all) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	auto& settings = ScreenReaderApp::GetInstance().GetSettings();
	auto& speech_settings = settings.speech;

	FindAnnouncementInHierarchy(out, obj /*, !m_isWhereAmIOperation, !m_isWhereAmIOperation*/);
	auto type = obj.GetType().value_or(EObjectType::UNKNOWN);
	if (!out.empty())
		Separate(out);
	out += GetObjectTypeName(type, out.empty() ? true : require_all);
	Separate(out);
	BuildStateAnnouncement(out, obj, require_all);

	if (settings.object_presentation.read_item_count && IsObjectDataElement(type)) {
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

void MessageBuilder::BuildStateAnnouncement(std::pmr::string& out, CObjectProxy obj, bool require_all) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	auto type = obj.GetType();
	if (!type)
		return;

	if (auto state = obj.GetState()) {
		GetObjectStateNames(out, *type, *state);
	}
}

void MessageBuilder::BuildValueAnnouncement(std::pmr::string& out, CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	auto value_provider = obj.GetAs<CValueProviderProxy>();
	if (auto current = value_provider.GetCurrent()) {
		std::format_to(std::back_inserter(out), "{}", *current);
	}
}

void MessageBuilder::BuildTextAnnouncement(std::pmr::string& out, CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	auto text_provider = obj.GetAs<CTextProviderProxy>();
	if (auto text = text_provider.GetText(text_provider.GetCursor().value_or(0), ETextGranularity::LINE)) {
		out += text->text;
	}
}
