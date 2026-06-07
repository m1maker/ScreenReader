/*
 * This file is part of the Screen Reader project.
 *
 * Copyright (C) 2025-2026  Georgiy Bondarenko (M_maker / m1maker) <georgijbondarenko248@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

module;
#include <cmath>
#include <sstream>
#include <utility>
module Core.MessageBuilder;
import Core.App;
import Core.Config;
import Core.Event;
import Core.KeyboardHandler;
import Core.KeyInfo;
import Core.KeyMeta;
import Core.Object;
import Core.ObjectMeta;
import Core.StateMeta;
import Core.Text;
import Proxies.Object;

MessageBuilder::MessageBuilder() : m_speechParameters(ScreenReaderApp::GetInstance().GetSettings().speech) {
	m_ssml = m_speechParameters.ssml;
}

/*
This method attempts to find a named object if the object that received the focus gain event doesn't have a
name. For example, Mate system info has list items, which also contain a bunch of obscure nested elements, and somewhere
in there are information labels.
*/
void MessageBuilder::FindAnnouncementInHierarchy(
	CMessage& message, CObjectProxy obj, bool recursive, bool collect_all_labels) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	std::string_view label_before{};
	auto type = obj.GetType().value_or(EObjectType::UNKNOWN);
	auto is_container = IsObjectInGroup(type, EObjectGroup::CONTAINER);
	auto is_feedback = IsObjectInGroup(type, EObjectGroup::FEEDBACK);
	auto should_search_for_label =
		is_container || (IsObjectInGroup(type, EObjectGroup::INPUT) || IsObjectInGroup(type, EObjectGroup::VALUE));
	if (should_search_for_label) {
		auto parent = obj.GetParent();
		auto index = obj.GetIndex();
		if (parent && index && *index > 0) {
			auto previous_index = (*index) - 1;
			auto previous_object = parent->GetChildAt(previous_index);
			if (previous_object) {
				auto previous_object_type = previous_object->GetType();
				if (previous_object_type && *previous_object_type == EObjectType::LABEL) {
					label_before = previous_object->GetName().value_or({});
					if (!label_before.empty())
						message.Append("{}", label_before);
				}
			}
		}
	}

	if (auto name = obj.GetName()) {
		if (!name->empty() && *name != label_before) {
			message.Append("{}", *name);
			if (is_container && !is_feedback)
				return;
		}
	}

	auto text_provider = obj.GetAs<CTextProviderProxy>();
	if (auto text = text_provider.GetText(text_provider.GetCursor().value_or(0), ETextGranularity::LINE)) {
		if (!text->text.empty()) {
			message.Append("{}", text->text);
			return;
		}
	}

	if (is_container)
		return;

	auto collect_labels_recursive = [&](auto& self, auto&& current) -> void {
		if (!current.IsValid())
			return;

		auto current_type = current.GetType().value_or(EObjectType::UNKNOWN);
		if (current_type == EObjectType::LABEL) {
			auto name = current.GetName().value_or("");
			if (!name.empty()) {
				message.Separate();
				message.Append("{}", name);
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

	if (collect_all_labels) {
		collect_labels_recursive(collect_labels_recursive, obj);
	}

	if (recursive) {
		if (auto children_count = obj.GetChildrenCount()) {
			for (auto i = 0; i < *children_count; ++i) {
				if (auto child = obj.GetChildAt(i))
					FindAnnouncementInHierarchy(message, *child, true, collect_all_labels);
			}
		}
	}
}

/*
This method tries to determine where the cursor has moved and returns a chunk of text.
Granularity is needed if the cursor has moved one character, in which case spelling should be enabled.
*/
void MessageBuilder::FindAnnouncementOfCursorPosition(
	CMessage& message, CTextProviderProxy provider, ETextGranularity& granularity) {
	if (!provider.IsValid()) [[unlikely]]
		return;

	auto current_cursor = provider.GetCursor();
	if (!current_cursor) {
		return;
	}

	auto& keyboard_handler = KeyboardHandler::GetInstance();
	auto keys_down = keyboard_handler.GetKeysDown();
	bool vertical_keys_down =
		keys_down[KEYCODE_UP] || keys_down[KEYCODE_DOWN] || keys_down[KEYCODE_PAGE_UP] || keys_down[KEYCODE_PAGE_DOWN];
	bool horizontal_keys_down =
		keys_down[KEYCODE_RIGHT] || keys_down[KEYCODE_LEFT] || keys_down[KEYCODE_HOME] || keys_down[KEYCODE_END];
	bool control_down = keyboard_handler.GetModifiers()[std::to_underlying(MODIFIER_CTRL)];

	bool typable_keys_down{false};
	bool editing_keys_down{false};
	for (size_t i = KEYCODE_NONE; i < KEYCODE_COUNT; ++i) {
		if (keys_down[i]) {
			if (IsKeycodeInGroup(static_cast<EKeycode>(i), EKeyGroup::TYPABLE)) {
				typable_keys_down = true;
				break;
			}
			else if (IsKeycodeInGroup(static_cast<EKeycode>(i), EKeyGroup::EDITING)) {
				editing_keys_down = true;
				break;
			}
		}
	}

	if (vertical_keys_down)
		granularity = ETextGranularity::LINE;
	else if (horizontal_keys_down) {
		granularity = control_down ? ETextGranularity::WORD : ETextGranularity::CHARACTER;
	}
	else if (typable_keys_down) {
		if (auto range = provider.GetText((*current_cursor) - 1, ETextGranularity::CHARACTER)) {
			message.Append("{}", range->text);
			return;
		}
	}

	if (auto keys_range = provider.GetText(current_cursor.value_or(0), granularity)) {
		message.Append("{}", keys_range->text);
	}
}

// Announcement builders
void MessageBuilder::BuildFocusAnnouncement(CMessage& message, CObjectProxy obj, bool require_all) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	BuildNameAnnouncement(message, obj);

	auto type = obj.GetType().value_or(EObjectType::UNKNOWN);
	message.Separate();
	message.ApplyUtteranceParameters(m_speechParameters.role);
	message.Append("{}", GetObjectTypeName(type));
	message.Separate();
	BuildStateAnnouncement(message, obj, require_all);

	if (ScreenReaderApp::GetInstance().GetSettings().object_presentation.read_item_count &&
		IsObjectInGroup(type, EObjectGroup::DATA_ELIMENT)) {
		auto index = obj.GetIndex().value_or(0) + 1;
		auto parent = obj.GetParent();
		if (parent || parent->IsValid()) {
			auto children_count = parent->GetChildrenCount().value_or(0);
			message.Separate();
			message.Append("{} of {}", index, children_count);
		}
	}
	message.Separate();
	BuildValueAnnouncement(message, obj);
	message.Separate();
	BuildTextAnnouncement(message, obj);

	BuildDescriptionAnnouncement(message, obj);
}

void MessageBuilder::BuildStateAnnouncement(CMessage& message, CObjectProxy obj, bool require_all) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	if (auto state = obj.GetState()) {
		message.ApplyUtteranceParameters(m_speechParameters.state);

		for (unsigned char i = 1; i < std::to_underlying(EObjectState::COUNT); ++i) {
			if (state->test(i)) {
				message.Separate();
				message.Append("{}", GetObjectStateName(static_cast<EObjectState>(i)));
			}
		}
	}
}

void MessageBuilder::BuildSelectionAnnouncement(CMessage& message, CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	auto selection_provider = obj.GetAs<CSelectionProviderProxy>();
	if (auto current_selected = selection_provider.GetChildAt(0)) {
		if (auto name = current_selected->GetName()) {
			message.ApplyUtteranceParameters(m_speechParameters.name);
			message.Append("{}", *name);
		}
	}
}

void MessageBuilder::BuildValueAnnouncement(CMessage& message, CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	auto value_provider = obj.GetAs<CValueProviderProxy>();
	if (auto current = value_provider.GetCurrent()) {
		message.ApplyUtteranceParameters(m_speechParameters.state);
		message.Append("{}", *current);
	}
}

void MessageBuilder::BuildNameAnnouncement(CMessage& message, CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	message.ApplyUtteranceParameters(m_speechParameters.name);
	FindAnnouncementInHierarchy(message, obj);
}

void MessageBuilder::BuildDescriptionAnnouncement(CMessage& message, CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	if (auto description = obj.GetDescription()) {
		message.ApplyUtteranceParameters(m_speechParameters.description);
		message.Append("{}", *description);
	}
}

void MessageBuilder::BuildTextAnnouncement(CMessage& message, CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	auto text_provider = obj.GetAs<CTextProviderProxy>();
	if (auto text = text_provider.GetText(text_provider.GetCursor().value_or(0), ETextGranularity::LINE)) {
		message.ApplyUtteranceParameters(m_speechParameters.text);
		message.Append("{}", text->text);
	}
}

void MessageBuilder::BuildTextSelectionAnnouncement(CMessage& message, CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	auto text_provider = obj.GetAs<CTextProviderProxy>();
	auto selected_text = text_provider.GetSelected();
	if (!selected_text)
		return;

	message.ApplyUtteranceParameters(m_speechParameters.text);
	message.Append("{}", selected_text->text);
	message.Separate();
	message.ApplyUtteranceParameters(m_speechParameters.state);
	message.Append("selected");
}

void MessageBuilder::BuildCursorAnnouncement(CMessage& message, CObjectProxy obj) {
	if (!obj.IsValid()) [[unlikely]]
		return;

	auto text_provider = obj.GetAs<CTextProviderProxy>();
	auto cursor = text_provider.GetCursor();
	if (!cursor) {
		return;
	}

	ETextGranularity granularity{ETextGranularity::CHARACTER};
	FindAnnouncementOfCursorPosition(message, text_provider, granularity);
}
