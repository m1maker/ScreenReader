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
import Core.Rect;
import Core.Text;

CObjectAtspi::CObjectAtspi(AtspiAccessible* accessible, SObjectAtspiData* data, std::pmr::memory_resource* pool)
	: m_accessible(accessible), m_data(data), m_pool(pool) {
	/*
		if (atspi_accessible_is_text(m_accessible))
			m_data->interfaces_mask |= EObjectInterfaceMaskSUPPORTS_TEXT;
		if (atspi_accessible_is_selection(m_accessible))
			m_data->interfaces_mask |= SUPPORTS_SELECTION;
		if (atspi_accessible_is_value(m_accessible))
			m_data->interfaces_mask |= SUPPORTS_VALUE;
	*/
}

void CObjectAtspi::UpdateCacheByEvent(EObjectEventType event) {
	/*
		if (!IsValid()) [[unlikely]]
			return;
		using enum EObjectEventType;
		switch (event) {
		case VALUE_CHANGED:
			InvalidateCache(min_value);
			InvalidateCache(max_value);
			InvalidateCache(current_value);
			break;
		case STATE_CHANGED:
			InvalidateCache(states);
			break;
		case CURSOR_MOVED:
			InvalidateCache(cursor);
			break;
		case CHILD_ADDED:
		case CHILD_REMOVED:
			InvalidateCache(children);
			break;
		case PARENT_UPDATED:
			InvalidateCache(parent);
			break;
		default:
			break;
		}
	*/
}

[[nodiscard]] auto CObjectAtspi::GetType() const -> ObjectResult<EObjectType> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	m_data->ResetLastError();
	AtspiRole role = atspi_accessible_get_role(m_accessible, &m_data->last_error);
	return GetObjectTypeFromAtspiRole(role);
}

[[nodiscard]] auto CObjectAtspi::GetState() const -> ObjectResult<ObjectStates> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	auto state_set = atspi_accessible_get_state_set(m_accessible);
	if (!state_set)
		return std::unexpected(EObjectError::FAIL);
	auto state = GetObjectStateFromAtspiStates(state_set);
	g_object_unref(state_set);
	return state;
}

[[nodiscard]] auto CObjectAtspi::GetParent() const -> ObjectResult<CObjectAtspi> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	m_data->ResetLastError();
	AtspiAccessible* native_parent = atspi_accessible_get_parent(m_accessible, &m_data->last_error);

	if (!native_parent)
		return std::unexpected(EObjectError::FAIL);

	auto parent_object =
		CObjectCache<AtspiAccessible, SObjectAtspiData>::GetInstance().GetOrCreate<CObjectAtspi>(native_parent);
	return parent_object;
}

[[nodiscard]] auto CObjectAtspi::GetChildAt(int index) const -> ObjectResult<CObjectAtspi> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	m_data->ResetLastError();
	AtspiAccessible* native_child = atspi_accessible_get_child_at_index(m_accessible, index, &m_data->last_error);

	if (!native_child)
		return std::unexpected(EObjectError::FAIL);

	auto child_object =
		CObjectCache<AtspiAccessible, SObjectAtspiData>::GetInstance().GetOrCreate<CObjectAtspi>(native_child);
	return child_object;
}

[[nodiscard]] auto CObjectAtspi::GetChildrenCount() const -> ObjectResult<int> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	m_data->ResetLastError();
	int child_count = atspi_accessible_get_child_count(m_accessible, &m_data->last_error);
	return child_count;
}

[[nodiscard]] auto CObjectAtspi::GetBounds() const -> ObjectResult<SRect> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);
	return std::unexpected(EObjectError::NOT_SUPPORTED);
}

[[nodiscard]] auto CObjectAtspi::GetIndex() const -> ObjectResult<int> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	m_data->ResetLastError();
	int index = atspi_accessible_get_index_in_parent(m_accessible, &m_data->last_error);
	return index;
}

[[nodiscard]] auto CObjectAtspi::GetApplicationName() const -> ObjectResult<std::string_view> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	m_data->ResetLastError();
	if (m_data->app_name) {
		g_free(m_data->app_name);
		m_data->app_name = nullptr;
	}
	m_data->app_name = atspi_accessible_get_toolkit_name(m_accessible, &m_data->last_error);
	return m_data->app_name;
}

[[nodiscard]] auto CObjectAtspi::GetName() const -> ObjectResult<std::string_view> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	m_data->ResetLastError();
	if (m_data->name) {
		g_free(m_data->name);
		m_data->name = nullptr;
	}
	m_data->name = atspi_accessible_get_name(m_accessible, &m_data->last_error);
	return m_data->name;
}

[[nodiscard]] auto CObjectAtspi::GetDescription() const -> ObjectResult<std::string_view> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	m_data->ResetLastError();

	if (m_data->description) {
		g_free(m_data->description);
		m_data->description = nullptr;
	}
	m_data->description = atspi_accessible_get_description(m_accessible, &m_data->last_error);
	return m_data->description;
}

[[nodiscard]] auto CObjectAtspi::GetCursor() const -> ObjectResult<int> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	SAtspiIface<AtspiText> text_interface(atspi_accessible_get_text_iface(m_accessible));
	if (!text_interface.pointer)
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	m_data->ResetLastError();

	return atspi_text_get_caret_offset(text_interface, &m_data->last_error);
}

[[nodiscard]] auto CObjectAtspi::GetText(int cursor, const ETextGranularity& granularity) const
	-> ObjectResult<STextRange> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	SAtspiIface<AtspiText> text_interface(atspi_accessible_get_text_iface(m_accessible));
	if (!text_interface.pointer)
		return std::unexpected(EObjectError::NOT_SUPPORTED);

	m_data->ResetLastError();

	if (m_data->last_text) {
		g_free(m_data->last_text);
		m_data->last_text = nullptr;
	}

	AtspiTextRange* pTextRange = atspi_text_get_string_at_offset(
		text_interface, cursor, GetAtspiTextGranularityFromTextGranularity(granularity), &m_data->last_error);
	if (!pTextRange)
		return std::unexpected(EObjectError::FAIL);

	m_data->last_text = pTextRange->content;

	auto text_range = GetTextRangeFromAtspiRange<AtspiTextRange>(*pTextRange);
	g_free(pTextRange);
	return text_range;
}

[[nodiscard]] auto CObjectAtspi::GetSelectedText() const -> ObjectResult<STextRange> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	SAtspiIface<AtspiText> text_interface(atspi_accessible_get_text_iface(m_accessible));
	if (!text_interface.pointer)
		return std::unexpected(EObjectError::NOT_SUPPORTED);

	m_data->ResetLastError();

	if (m_data->last_text) {
		g_free(m_data->last_text);
		m_data->last_text = nullptr;
	}

	AtspiRange* pRange = atspi_text_get_selection(text_interface, 0, &m_data->last_error);
	if (!pRange)
		return std::unexpected(EObjectError::FAIL);

	m_data->ResetLastError();
	gchar* pText = atspi_text_get_text(text_interface, pRange->start_offset, pRange->end_offset, &m_data->last_error);
	m_data->last_text = pText;
	STextRange text_range = {.start = pRange->start_offset, .end = pRange->end_offset, .text = pText};
	g_free(pRange);
	return text_range;
}

[[nodiscard]] auto CObjectAtspi::GetSelectedChildAt(int index) const -> ObjectResult<CObjectAtspi> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);
	SAtspiIface<AtspiSelection> selection_interface(atspi_accessible_get_selection_iface(m_accessible));
	if (!selection_interface.pointer)
		return std::unexpected(EObjectError::NOT_SUPPORTED);

	m_data->ResetLastError();
	AtspiAccessible* native_selected_child =
		atspi_selection_get_selected_child(selection_interface, index, &m_data->last_error);
	if (!native_selected_child)
		return std::unexpected(EObjectError::FAIL);

	auto selected_child_object =
		CObjectCache<AtspiAccessible, SObjectAtspiData>::GetInstance().GetOrCreate<CObjectAtspi>(native_selected_child);
	return selected_child_object;
}

[[nodiscard]] auto CObjectAtspi::GetSelectedChildrenCount() const -> ObjectResult<int> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);
	SAtspiIface<AtspiSelection> selection_interface(atspi_accessible_get_selection_iface(m_accessible));
	if (!selection_interface.pointer)
		return std::unexpected(EObjectError::NOT_SUPPORTED);

	m_data->ResetLastError();
	int selected_child_count = atspi_selection_get_n_selected_children(selection_interface, &m_data->last_error);
	return selected_child_count;
}

[[nodiscard]] auto CObjectAtspi::GetMinValue() const -> ObjectResult<double> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	SAtspiIface<AtspiValue> value_interface(atspi_accessible_get_value_iface(m_accessible));
	if (!value_interface.pointer)
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	m_data->ResetLastError();

	return atspi_value_get_minimum_value(value_interface, &m_data->last_error);
}

[[nodiscard]] auto CObjectAtspi::GetMaxValue() const -> ObjectResult<double> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	SAtspiIface<AtspiValue> value_interface(atspi_accessible_get_value_iface(m_accessible));
	if (!value_interface.pointer)
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	m_data->ResetLastError();

	return atspi_value_get_maximum_value(value_interface, &m_data->last_error);
}

[[nodiscard]] auto CObjectAtspi::GetCurrentValue() const -> ObjectResult<double> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	SAtspiIface<AtspiValue> value_interface(atspi_accessible_get_value_iface(m_accessible));
	if (!value_interface.pointer)
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	m_data->ResetLastError();

	return atspi_value_get_current_value(value_interface, &m_data->last_error);
}

[[nodiscard]] auto CObjectAtspi::GetActionCount() const -> ObjectResult<int> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	SAtspiIface<AtspiAction> action_interface(atspi_accessible_get_action_iface(m_accessible));
	if (!action_interface.pointer)
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	m_data->ResetLastError();

	return atspi_action_get_n_actions(action_interface, &m_data->last_error);
}

[[nodiscard]] auto CObjectAtspi::GetActionType(int number) const -> ObjectResult<EObjectAction> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	SAtspiIface<AtspiAction> action_interface(atspi_accessible_get_action_iface(m_accessible));
	if (!action_interface.pointer)
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	m_data->ResetLastError();

	return std::unexpected(EObjectError::NOT_SUPPORTED);
}

[[nodiscard]] auto CObjectAtspi::GetActionName(int number) const -> ObjectResult<std::string_view> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	SAtspiIface<AtspiAction> action_interface(atspi_accessible_get_action_iface(m_accessible));
	if (!action_interface.pointer)
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	m_data->ResetLastError();

	if (m_data->action_name)
		g_free(m_data->action_name);

	m_data->action_name = atspi_action_get_action_name(action_interface, number, &m_data->last_error);
	return m_data->action_name;
}

[[nodiscard]] auto CObjectAtspi::DoAction(int number) -> ObjectResult<> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	SAtspiIface<AtspiAction> action_interface(atspi_accessible_get_action_iface(m_accessible));
	if (!action_interface.pointer)
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	m_data->ResetLastError();

	auto _ = atspi_action_do_action(action_interface, number, &m_data->last_error);
	return ObjectResult<>();
}
