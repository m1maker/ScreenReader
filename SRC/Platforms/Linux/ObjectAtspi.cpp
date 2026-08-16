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

template <typename T> using AtspiInterfaceGetFunction = T* (*)(AtspiAccessible*);
template <typename T, typename I> using AtspiInterfaceGetValue = T (*)(I*, GError**);

template <typename T, AtspiInterfaceGetFunction<T> Function>
static inline void GetInterfaceIfNeeded(AtspiAccessible* accessible, T*& interface) noexcept {
	if (interface != nullptr)
		return;

	interface = Function(accessible);
}

template <typename T, typename I, AtspiInterfaceGetValue<T, I> Function>
static inline void GetInterfaceValueIfNeeded(I* interface, GError** error, T& value) noexcept {
	if (value != 0)
		return;

	value = Function(interface, error);
}

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
			slot->parent = TObjectCache<AtspiAccessible*>::GetInstance().GetOrCreate(native_parent);
	}
	if (request->mask.test(std::to_underlying(EObjectFetchValue::CHILDREN))) {
		auto native_children_count = atspi_accessible_get_child_count(native_handle, nullptr);
		slot->ReserveMemory(native_children_count, slot->children);
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
			slot->MakeCopy(toolkit_name, slot->toolkit_name);
		g_free(toolkit_name);
	}
	if (request->mask.test(std::to_underlying(EObjectFetchValue::NAME))) {
		auto name = atspi_accessible_get_name(native_handle, nullptr);
		if (!name) {
			slot->name = std::unexpected(EObjectError::FAIL);
		}
		else
			slot->MakeCopy(name, slot->name);
		g_free(name);
	}
	if (request->mask.test(std::to_underlying(EObjectFetchValue::DESCRIPTION))) {
		auto description = atspi_accessible_get_description(native_handle, nullptr);
		if (!description) {
			slot->description = std::unexpected(EObjectError::FAIL);
		}
		else
			slot->MakeCopy(description, slot->description);
		g_free(description);
	}

	AtspiText* text_interface{nullptr};
	AtspiSelection* selection_interface{nullptr};
	AtspiValue* value_interface{nullptr};
	AtspiAction* action_interface{nullptr};
	GArray* relation_set{nullptr};

	int action_count{0};
	if (request->mask.test(std::to_underlying(EObjectFetchValue::SELECTED_CHILDREN))) {
		GetInterfaceIfNeeded<AtspiSelection, atspi_accessible_get_selection_iface>(native_handle, selection_interface);
		auto native_selected_children_count = atspi_selection_get_n_selected_children(selection_interface, nullptr);
		slot->ReserveMemory(native_selected_children_count, slot->selected_children);
		for (auto i = 0; i < native_selected_children_count; ++i) {
			auto native_selected_child = atspi_selection_get_selected_child(selection_interface, i, nullptr);
			if (!native_selected_child)
				continue;
			slot->children->operator[](i) =
				TObjectCache<AtspiAccessible*>::GetInstance().GetOrCreate(native_selected_child);
		}
	}
	if (request->mask.test(std::to_underlying(EObjectFetchValue::ACTION_TYPES))) {
		GetInterfaceIfNeeded<AtspiAction, atspi_accessible_get_action_iface>(native_handle, action_interface);
		GetInterfaceValueIfNeeded<int, AtspiAction, atspi_action_get_n_actions>(
			action_interface, nullptr, action_count);

		slot->action_types = std::unexpected(EObjectError::NOT_SUPPORTED);
	}
	if (request->mask.test(std::to_underlying(EObjectFetchValue::ACTION_NAMES))) {
		GetInterfaceIfNeeded<AtspiAction, atspi_accessible_get_action_iface>(native_handle, action_interface);
		GetInterfaceValueIfNeeded<int, AtspiAction, atspi_action_get_n_actions>(
			action_interface, nullptr, action_count);

		slot->ReserveMemory(action_count, slot->action_names);
		for (auto i = 0; i < action_count; ++i) {
			auto action_name = atspi_action_get_action_name(action_interface, i, nullptr);
			if (!action_name)
				continue;

			slot->MakeCopy(action_name, slot->action_names->operator[](i));
			g_free(action_name);
		}
	}
	if (request->mask.test(std::to_underlying(EObjectFetchValue::ACTION_DESCRIPTIONS))) {
		GetInterfaceIfNeeded<AtspiAction, atspi_accessible_get_action_iface>(native_handle, action_interface);
		GetInterfaceValueIfNeeded<int, AtspiAction, atspi_action_get_n_actions>(
			action_interface, nullptr, action_count);

		slot->ReserveMemory(action_count, slot->action_descriptions);
		for (auto i = 0; i < action_count; ++i) {
			auto action_description = atspi_action_get_action_description(action_interface, i, nullptr);
			if (!action_description)
				continue;

			slot->MakeCopy(action_description, slot->action_descriptions->operator[](i));
			g_free(action_description);
		}
	}
	if (request->mask.test(std::to_underlying(EObjectFetchValue::ACTION_HOTKEYS))) {
		GetInterfaceIfNeeded<AtspiAction, atspi_accessible_get_action_iface>(native_handle, action_interface);
		GetInterfaceValueIfNeeded<int, AtspiAction, atspi_action_get_n_actions>(
			action_interface, nullptr, action_count);

		slot->action_hotkeys = std::unexpected(EObjectError::NOT_SUPPORTED);
	}
	if (request->mask.test(std::to_underlying(EObjectFetchValue::ACTION_HOTKEY_STRINGS))) {
		GetInterfaceIfNeeded<AtspiAction, atspi_accessible_get_action_iface>(native_handle, action_interface);
		GetInterfaceValueIfNeeded<int, AtspiAction, atspi_action_get_n_actions>(
			action_interface, nullptr, action_count);

		slot->ReserveMemory(action_count, slot->action_hotkey_strings);
		for (auto i = 0; i < action_count; ++i) {
			auto action_hotkey_string = atspi_action_get_key_binding(action_interface, i, nullptr);
			if (!action_hotkey_string)
				continue;

			slot->MakeCopy(action_hotkey_string, slot->action_hotkey_strings->operator[](i));
			g_free(action_hotkey_string);
		}
	}
	if (request->mask.test(std::to_underlying(EObjectFetchValue::VALUE_MIN))) {
		GetInterfaceIfNeeded<AtspiValue, atspi_accessible_get_value_iface>(native_handle, value_interface);

		slot->value_min = atspi_value_get_minimum_value(value_interface, nullptr);
	}
	if (request->mask.test(std::to_underlying(EObjectFetchValue::VALUE_MAX))) {
		GetInterfaceIfNeeded<AtspiValue, atspi_accessible_get_value_iface>(native_handle, value_interface);

		slot->value_max = atspi_value_get_maximum_value(value_interface, nullptr);
	}
	if (request->mask.test(std::to_underlying(EObjectFetchValue::VALUE_CURRENT))) {
		GetInterfaceIfNeeded<AtspiValue, atspi_accessible_get_value_iface>(native_handle, value_interface);

		slot->value_current = atspi_value_get_current_value(value_interface, nullptr);
	}

	if (text_interface)
		g_object_unref(text_interface);
	if (selection_interface)
		g_object_unref(selection_interface);
	if (action_interface)
		g_object_unref(action_interface);
	if (value_interface)
		g_object_unref(value_interface);
	if (relation_set)
		g_array_unref(relation_set);

	slot->mask = request->mask;
}
