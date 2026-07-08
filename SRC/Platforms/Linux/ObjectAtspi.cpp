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

// AT-SPI object implementation.
module;
#include <atspi/atspi.h>
#include <expected>
#include <string>
#include <utility>
module Platforms.Linux.Object;
import Core.ObjectCache;
import Core.Rect;
import Core.Text;

void ObjectAtspiFetch(const SObjectFetchRequest* request) noexcept {
	if (!request) [[unlikely]]
		return;

	auto slot = request->slot;
	auto native_handle = static_cast<AtspiAccessible*>(request->native_handle);
	if (request->mask.test(std::to_underlying(EObjectFetchValue::TYPE))) {
		auto native_role = atspi_accessible_get_role(native_handle, nullptr);
		slot->type = GetObjectTypeFromAtspiRole(native_role);
	}
	if (request->mask.test(std::to_underlying(EObjectFetchValue::STATES))) {
		auto native_states = atspi_accessible_get_state_set(native_handle);
		slot->states = GetMergedObjectStateFromAtspiStates(native_states).states;
		g_object_unref(native_states);
	}
	if (request->mask.test(std::to_underlying(EObjectFetchValue::CAPABILITIES))) {
		auto native_states = atspi_accessible_get_state_set(native_handle);
		slot->capabilities = GetMergedObjectStateFromAtspiStates(native_states).capabilities;
		g_object_unref(native_states);
	}
	if (request->mask.test(std::to_underlying(EObjectFetchValue::PARENT))) {
		auto native_parent = atspi_accessible_get_parent(native_handle, nullptr);
		if (!native_parent) {
			slot->parent = std::unexpected(EObjectError::FAIL);
		}
		else
			slot->parent = TObjectCache<AtspiAccessible*>::GetInstance().GetOrCreate(native_handle);
	}
	if (request->mask.test(std::to_underlying(EObjectFetchValue::CHILDREN))) {
		auto native_children_count = atspi_accessible_get_child_count(native_handle, nullptr);
		slot->children->resize(native_children_count);
		for (auto i = 0; i < native_children_count; ++i) {
			auto native_child = atspi_accessible_get_child_at_index(native_handle, i, nullptr);
			if (!native_child)
				continue;
			slot->children->operator[](i) = TObjectCache<AtspiAccessible*>::GetInstance().GetOrCreate(native_child);
		}
	}
	if (request->mask.test(std::to_underlying(EObjectFetchValue::INDEX))) {
		auto native_index = atspi_accessible_get_index_in_parent(native_handle, nullptr);
		slot->index = native_index;
	}
	if (request->mask.test(std::to_underlying(EObjectFetchValue::TOOLKIT_NAME))) {
		auto toolkit_name = atspi_accessible_get_toolkit_name(native_handle, nullptr);
		if (!toolkit_name) {
			slot->toolkit_name = std::unexpected(EObjectError::FAIL);
		}
		else
			slot->toolkit_name = toolkit_name;
		g_free(toolkit_name);
	}
	if (request->mask.test(std::to_underlying(EObjectFetchValue::NAME))) {
		auto name = atspi_accessible_get_name(native_handle, nullptr);
		if (!name) {
			slot->name = std::unexpected(EObjectError::FAIL);
		}
		else
			slot->name = name;
		g_free(name);
	}
	if (request->mask.test(std::to_underlying(EObjectFetchValue::DESCRIPTION))) {
		auto description = atspi_accessible_get_description(native_handle, nullptr);
		if (!description) {
			slot->name = std::unexpected(EObjectError::FAIL);
		}
		else
			slot->description = description;
		g_free(description);
	}
}
