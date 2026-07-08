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
#include <bitset>
#include <string_view>
#include <utility>
export module Core.ObjectProviderMeta;
import Core.Object;

struct SObjectProviderMeta final {
	std::string_view name;
	ObjectFetchMask value_flags;
};

[[nodiscard]] static consteval auto InitializeMeta(EObjectProvider provider) -> SObjectProviderMeta {
	SObjectProviderMeta meta;

	switch (provider) {
	case EObjectProvider::UNKNOWN:
		meta.name = "unknown";
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::UNKNOWN));
		break;
	case EObjectProvider::MAIN:
		meta.name = "main";
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::TYPE));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::STATES));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::CAPABILITIES));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::PARENT));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::CHILDREN));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::INDEX));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::BOUNDS));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::TOOLKIT_NAME));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::TOOLKIT_VERSION));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::NAME));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::DESCRIPTION));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::HELP_TEXT));
		break;
	case EObjectProvider::TEXT:
		meta.name = "text";
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::TEXT_CURSOR));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::TEXT_LENGTH));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::TEXT));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::TEXT_SELECTION));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::TEXT_SELECTION_RANGE));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::TEXT_BY_GRANULARITY));
		break;
	case EObjectProvider::SELECTION:
		meta.name = "selection";
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::SELECTED_CHILDREN));
		break;

	case EObjectProvider::ACTION:
		meta.name = "action";
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::ACTION_TYPES));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::ACTION_NAMES));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::ACTION_DESCRIPTIONS));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::ACTION_HOTKEYS));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::ACTION_HOTKEY_STRINGS));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::ACTION_DO));
		break;

	case EObjectProvider::VALUE:
		meta.name = "value";
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::VALUE_MIN));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::VALUE_MAX));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::VALUE_CURRENT));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::VALUE_STEP));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::VALUE_STRING));
		break;
	case EObjectProvider::RELATION:
		meta.name = "relation";
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::RELATION_TYPES));
		meta.value_flags.set(std::to_underlying(EObjectFetchValue::RELATION_TARGETS));
		break;
	}

	return meta;
}

using ObjectProviderMetaArray = std::array<SObjectProviderMeta, std::to_underlying(EObjectProvider::COUNT)>;

[[nodiscard]] static consteval auto InitializeMetaArray() -> ObjectProviderMetaArray {
	ObjectProviderMetaArray array;
	for (size_t i = 0; i < static_cast<size_t>(EObjectProvider::COUNT); ++i) {
		auto meta = InitializeMeta(static_cast<EObjectProvider>(i));
		array[i] = meta;
	}
	return array;
}

static constexpr ObjectProviderMetaArray cObjectProviderMetadata = InitializeMetaArray();

export [[nodiscard]] constexpr auto GetObjectProviderName(EObjectProvider provider) -> std::string_view {
	auto index = static_cast<size_t>(provider);
	if (index < 0 || index > cObjectProviderMetadata.size()) [[unlikely]]
		return "unknown";

	return cObjectProviderMetadata[index].name;
}

export [[nodiscard]] constexpr auto GetObjectProviderValueMask(EObjectProvider provider) -> ObjectFetchMask {
	auto index = static_cast<size_t>(provider);
	if (index < 0 || index > cObjectProviderMetadata.size()) [[unlikely]]
		return {};

	return cObjectProviderMetadata[index].value_flags;
}
