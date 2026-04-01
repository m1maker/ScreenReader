module;
#include <cmath>
#include <sstream>
module Core.MessageBuilder;
import Core.App;
import Core.Config;
import Core.Event;
import Core.KeyboardHandler;
import Core.KeyInfo;
import Core.Object;
import Core.Text;
import Proxies.Object;
import Traits.SpeechEngine;

MessageBuilder::MessageBuilder() : m_speechParameters(ScreenReaderApp::GetInstance().GetSettings().speech) {
	m_ssml = m_speechParameters.ssml;
}

/*
This method attempts to find a named object if the object that received the focus gain event doesn't have a
name. For example, Mate system info has list items, which also contain a bunch of obscure nested elements, and somewhere
in there are information labels.
*/
void MessageBuilder::FindAnnouncementInHierarchy(CObjectProxy obj, bool recursive, bool collect_all_labels) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	if (auto name = obj.GetName()) {
		if (!name->empty()) {
			Append(*name);
			return;
		}
	}

	auto text_provider = obj.GetAs<CTextProviderProxy>();
	if (auto text = text_provider.GetText(text_provider.GetCursor().value_or(0), ETextGranularity::LINE)) {
		if (!text->text.empty()) {
			Append(text->text);
			return;
		}
	}

	auto collect_labels_recursive = [&](auto& self, auto&& current) -> void {
		if (!current.IsValid())
			return;

		auto current_type = current.GetType().value_or(EObjectType::UNKNOWN);
		if (current_type == EObjectType::LABEL) {
			auto name = current.GetName().value_or("");
			if (!name.empty()) {
				if (!m_content.empty())
					Separate();
				Append(name);
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
		if (!m_content.empty())
			return;
	}

	if (recursive) {
		if (auto children_count = obj.GetChildrenCount()) {
			for (auto i = 0; i < *children_count; ++i) {
				if (auto child = obj.GetChildAt(i))
					FindAnnouncementInHierarchy(*child, true, collect_all_labels);
			}
		}
	}
}

/*
This method tries to determine where the cursor has moved and returns a chunk of text.
Granularity is needed if the cursor has moved one character, in which case spelling should be enabled.
*/
void MessageBuilder::FindAnnouncementOfCursorPosition(CTextProviderProxy provider, ETextGranularity& granularity) {
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
	else if (auto range = provider.GetText((*current_cursor) - 1, ETextGranularity::CHARACTER)) {
		Append(range->text);
		return;
	}

	if (auto keys_range = provider.GetText(current_cursor.value_or(0), granularity)) {
		Append(keys_range->text);
	}
}

// Announcement builders
void MessageBuilder::BuildFocusAnnouncement(CObjectProxy obj, bool require_all) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	TScopedBegin _(*this);
	BuildNameAnnouncement(obj);

	auto type = obj.GetType().value_or(EObjectType::UNKNOWN);
	if (!m_content.empty())
		Separate();
	ApplyUtteranceParameters(m_speechParameters.role);
	Append(GetObjectTypeName(type, m_content.empty() ? true : require_all));
	Separate();
	BuildStateAnnouncement(obj, require_all);

	if (ScreenReaderApp::GetInstance().GetSettings().object_presentation.read_item_count && IsObjectDataElement(type)) {
		auto index = obj.GetIndex().value_or(0) + 1;
		auto parent = obj.GetParent();
		if (parent || parent->IsValid()) {
			auto children_count = parent->GetChildrenCount().value_or(0);
			Separate();
			std::format_to(std::back_inserter(m_content), "{}{} of {}", cSeparator, index, children_count);
		}
	}
	Separate();
	BuildValueAnnouncement(obj);
	Separate();
	BuildTextAnnouncement(obj);

	BuildDescriptionAnnouncement(obj);
}

void MessageBuilder::BuildStateAnnouncement(CObjectProxy obj, bool require_all) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	TScopedBegin _(*this);
	auto type = obj.GetType();
	if (!type)
		return;

	if (auto state = obj.GetState()) {
		ApplyUtteranceParameters(m_speechParameters.state);
		GetObjectStateNames(m_content, *type, *state);
	}
}

void MessageBuilder::BuildSelectionAnnouncement(CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	TScopedBegin _(*this);
	auto selection_provider = obj.GetAs<CSelectionProviderProxy>();
	if (auto current_selected = selection_provider.GetChildAt(0)) {
		if (auto name = current_selected->GetName()) {
			ApplyUtteranceParameters(m_speechParameters.name);
			Append(*name);
		}
	}
}

void MessageBuilder::BuildValueAnnouncement(CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	TScopedBegin _(*this);
	auto value_provider = obj.GetAs<CValueProviderProxy>();
	if (auto current = value_provider.GetCurrent()) {
		ApplyUtteranceParameters(m_speechParameters.state);
		std::format_to(std::back_inserter(m_content), "{}", *current);
	}
}

void MessageBuilder::BuildNameAnnouncement(CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	TScopedBegin _(*this);
	ApplyUtteranceParameters(m_speechParameters.name);
	FindAnnouncementInHierarchy(obj);
}

void MessageBuilder::BuildDescriptionAnnouncement(CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	TScopedBegin _(*this);
	if (auto description = obj.GetDescription()) {
		ApplyUtteranceParameters(m_speechParameters.description);
		Append(*description);
	}
}

void MessageBuilder::BuildTextAnnouncement(CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	TScopedBegin _(*this);
	auto text_provider = obj.GetAs<CTextProviderProxy>();
	if (auto text = text_provider.GetText(text_provider.GetCursor().value_or(0), ETextGranularity::LINE)) {
		ApplyUtteranceParameters(m_speechParameters.text);
		Append(text->text);
	}
}

void MessageBuilder::BuildTextSelectionAnnouncement(CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	TScopedBegin _(*this);
	auto text_provider = obj.GetAs<CTextProviderProxy>();
	auto selected_text = text_provider.GetSelected();
	if (!selected_text)
		return;

	ApplyUtteranceParameters(m_speechParameters.text);
	Append(selected_text->text);
	Separate();
	ApplyUtteranceParameters(m_speechParameters.state);
	Append("selected");
}

void MessageBuilder::BuildCursorAnnouncement(CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	TScopedBegin _(*this);
	auto text_provider = obj.GetAs<CTextProviderProxy>();
	auto cursor = text_provider.GetCursor();
	if (!cursor) {
		return;
	}

	ETextGranularity granularity{ETextGranularity::CHARACTER};
	FindAnnouncementOfCursorPosition(text_provider, granularity);
}
