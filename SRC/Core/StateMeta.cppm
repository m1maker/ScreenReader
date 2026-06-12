/*
 * This file is part of the Screen Reader project.
 *
 * Copyright (C) 2026  Georgiy Bondarenko (M_maker / m1maker) <georgijbondarenko248@gmail.com>
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
#include <array>
#include <string_view>
#include <utility>
export module Core.StateMeta;
import Core.Object;

struct SStateMeta final {
	std::string_view speech_name;
	EObjectCapability required_capability{EObjectCapability::NO};
	bool always_announce{false};
};

[[nodiscard]] static consteval auto InitializeMeta(EObjectState state) -> SStateMeta {
	SStateMeta meta;

	using enum EObjectState;
	switch (state) {
	case NO:
	case COUNT:
		meta.speech_name = "no";
		break;
	case ACTIVE:
		meta.speech_name = "active";
		break;
	case BUSY:
		meta.speech_name = "busy";
		meta.always_announce = true;
		break;
	case CHECKED:
		meta.speech_name = "checked";
		meta.required_capability = EObjectCapability::CHECKABLE;
		meta.always_announce = true;
		break;
	case COLLAPSED:
		meta.speech_name = "collapsed";
		meta.required_capability = EObjectCapability::EXPANDABLE;
		meta.always_announce = true;
		break;
	case DEFAULT:
		meta.speech_name = "default";
		meta.always_announce = true;
		break;
	case DEFUNCT:
		meta.speech_name = "defunct";
		meta.always_announce = true;
		break;
	case DRAGGING:
		meta.speech_name = "dragging";
		meta.required_capability = EObjectCapability::DRAGGABLE;
		meta.always_announce = true;
		break;
	case ENABLED:
		meta.speech_name = "enabled";
		break;
	case EXPANDED:
		meta.speech_name = "expanded";
		meta.required_capability = EObjectCapability::EXPANDABLE;
		meta.always_announce = true;
		break;
	case FOCUSED:
		meta.speech_name = "focused";
		meta.required_capability = EObjectCapability::FOCUSABLE;
		break;
	case HOVERED:
		meta.speech_name = "hovered";
		break;
	case INDETERMINATE:
		meta.speech_name = "indeterminate";
		meta.required_capability = EObjectCapability::CHECKABLE;
		meta.always_announce = true;
		break;
	case INVALID:
		meta.speech_name = "invalid";
		meta.always_announce = true;
		break;
	case LINKED:
		meta.speech_name = "linked";
		meta.always_announce = true;
		break;
	case LOADING:
		meta.speech_name = "loading";
		meta.always_announce = true;
		break;
	case MODAL:
		meta.speech_name = "modal";
		meta.always_announce = true;
		break;
	case OFFSCREEN:
		meta.speech_name = "offscreen";
		meta.always_announce = true;
		break;
	case PRESSED:
		meta.speech_name = "pressed";
		meta.always_announce = true;
		break;
	case READONLY:
		meta.speech_name = "readonly";
		meta.always_announce = true;
		break;
	case REQUIRED:
		meta.speech_name = "required";
		meta.required_capability = EObjectCapability::EDITABLE;
		meta.always_announce = true;
		break;
	case SELECTED:
		meta.speech_name = "selected";
		meta.required_capability = EObjectCapability::SELECTABLE;
		meta.always_announce = true;
		break;
	case VISITED:
		meta.speech_name = "visited";
		meta.always_announce = true;
		break;
	case VISIBLE:
		meta.speech_name = "visible";
		break;
	case CLIPPED:
		meta.speech_name = "clipped";
		meta.always_announce = true;
		break;
	case HIDDEN:
		meta.speech_name = "hidden";
		meta.always_announce = true;
		break;
	}

	return meta;
}

using StateMetaArray = std::array<SStateMeta, std::to_underlying(EObjectState::COUNT)>;

[[nodiscard]] static consteval auto InitializeMetaArray() -> StateMetaArray {
	StateMetaArray array;
	for (size_t i = 0; i < static_cast<size_t>(EObjectState::COUNT); ++i) {
		auto meta = InitializeMeta(static_cast<EObjectState>(i));
		array[i] = meta;
	}
	return array;
}

static constexpr StateMetaArray cStateMetadata = InitializeMetaArray();

export [[nodiscard]] constexpr auto GetObjectStateName(EObjectState state) -> std::string_view {
	auto index = static_cast<size_t>(state);
	if (index < 0 || index > cStateMetadata.size()) [[unlikely]]
		return "no";

	return cStateMetadata[index].speech_name;
}

export [[nodiscard]] constexpr auto ShouldObjectStateBeAnnounced(EObjectState state) -> bool {
	auto index = static_cast<size_t>(state);
	if (index < 0 || index > cStateMetadata.size()) [[unlikely]]
		return false;

	return cStateMetadata[index].always_announce;
}
