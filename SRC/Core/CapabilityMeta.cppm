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
export module Core.CapabilityMeta;
import Core.Object;

struct SCapabilityMeta final {
	std::string_view speech_name;
	ObjectStateMask compatible_states;
	bool always_announce{false};
};

[[nodiscard]] static consteval auto InitializeMeta(EObjectCapability capability) -> SCapabilityMeta {
	SCapabilityMeta meta;

	using enum EObjectCapability;
	switch (capability) {
	case NO:
	case COUNT:
		meta.speech_name = "no";
		break;
	case ANIMATED:
		meta.speech_name = "animated";
		break;
	case AUTO_FILL_AVAILABLE:
		meta.speech_name = "auto fill available";
		meta.always_announce = true;
		break;
	case CHECKABLE:
		meta.speech_name = "checkable";
		meta.compatible_states.set(std::to_underlying(EObjectState::CHECKED));
		break;
	case CLICKABLE:
		meta.speech_name = "clickable";
		meta.always_announce = true;
		break;
	case DRAGGABLE:
		meta.speech_name = "draggable";
		meta.compatible_states.set(std::to_underlying(EObjectState::DRAGGING));
		break;
	case EDITABLE:
		meta.speech_name = "editable";
		break;
	case EXPANDABLE:
		meta.speech_name = "expandable";
		meta.compatible_states.set(std::to_underlying(EObjectState::COLLAPSED));
		meta.compatible_states.set(std::to_underlying(EObjectState::EXPANDED));
		break;
	case FOCUSABLE:
		meta.speech_name = "focusable";
		meta.compatible_states.set(std::to_underlying(EObjectState::FOCUSED));
		break;
	case HAS_POPUP:
		meta.speech_name = "has popup";
		meta.always_announce = true;
		break;
	case HORIZONTAL:
		meta.speech_name = "horizontal";
		break;
	case MAPPABLE:
		meta.speech_name = "mappable";
		break;
	case MULTI_LINE:
		meta.speech_name = "multi line";
		meta.always_announce = true;
		break;
	case MULTI_SELECTABLE:
		meta.speech_name = "multi selectable";
		meta.always_announce = true;
		break;
	case MOVEABLE:
		meta.speech_name = "moveable";
		break;
	case PINNABLE:
		meta.speech_name = "pinnable";
		break;
	case RESIZABLE:
		meta.speech_name = "resizable";
		break;
	case SELECTABLE:
		meta.speech_name = "selectable";
		meta.compatible_states.set(std::to_underlying(EObjectState::SELECTED));
		break;
	case SECURE:
		meta.speech_name = "secure";
		meta.always_announce = true;
		break;
	case SORTABLE:
		meta.speech_name = "sortable";
		break;
	case TOUCH_OPTIMIZED:
		meta.speech_name = "touch optimized";
		break;
	case VERTICAL:
		meta.speech_name = "vertical";
		break;
	case HAS_TOOLTIP:
		meta.speech_name = "has tooltip";
		break;
	case LIVE_REGION:
		meta.speech_name = "live region";
		break;
	case PROTECTED:
		meta.speech_name = "protected";
		meta.always_announce = true;
		break;
	case SENSITIVE:
		meta.speech_name = "sensitive";
		break;
	}

	return meta;
}

using CapabilityMetaArray = std::array<SCapabilityMeta, std::to_underlying(EObjectCapability::COUNT)>;

[[nodiscard]] static consteval auto InitializeMetaArray() -> CapabilityMetaArray {
	CapabilityMetaArray array;
	for (size_t i = 0; i < static_cast<size_t>(EObjectCapability::COUNT); ++i) {
		auto meta = InitializeMeta(static_cast<EObjectCapability>(i));
		array[i] = meta;
	}
	return array;
}

static constexpr CapabilityMetaArray cCapabilityMetadata = InitializeMetaArray();

export [[nodiscard]] constexpr auto GetObjectCapabilityName(EObjectCapability capability) -> std::string_view {
	auto index = static_cast<size_t>(capability);
	if (index < 0 || index > cCapabilityMetadata.size()) [[unlikely]]
		return "no";

	return cCapabilityMetadata[index].speech_name;
}

export [[nodiscard]] constexpr auto ShouldObjectCapabilityBeAnnounced(EObjectCapability capability) -> bool {
	auto index = static_cast<size_t>(capability);
	if (index < 0 || index > cCapabilityMetadata.size()) [[unlikely]]
		return false;

	return cCapabilityMetadata[index].always_announce;
}
