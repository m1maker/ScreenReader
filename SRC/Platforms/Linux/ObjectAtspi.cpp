// AT-SPI object implementation.
module;
#include <Core/Defer.h>
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

[[nodiscard]] auto CObjectAtspi::GetState() const -> ObjectResult<unsigned long long> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	AtspiStateSet* state_set = atspi_accessible_get_state_set(m_accessible);
	if (!state_set)
		return std::unexpected(EObjectError::FAIL);
	defer(g_object_unref(state_set));
	return GetObjectStateFromAtspiStates(state_set);
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

[[nodiscard]] auto CObjectAtspi::GetApplicationName() const -> ObjectResult<std::string> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	m_data->ResetLastError();
	CGlibString application_name(atspi_accessible_get_toolkit_name(m_accessible, &m_data->last_error));
	return application_name;
}

[[nodiscard]] auto CObjectAtspi::GetName() const -> ObjectResult<std::string> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	m_data->ResetLastError();
	CGlibString name(atspi_accessible_get_name(m_accessible, &m_data->last_error));
	return name;
}

[[nodiscard]] auto CObjectAtspi::GetDescription() const -> ObjectResult<std::string> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	m_data->ResetLastError();

	CGlibString description(atspi_accessible_get_description(m_accessible, &m_data->last_error));
	return description;
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
	-> ObjectResult<STextRange<std::string>> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	SAtspiIface<AtspiText> text_interface(atspi_accessible_get_text_iface(m_accessible));
	if (!text_interface.pointer)
		return std::unexpected(EObjectError::NOT_SUPPORTED);

	m_data->ResetLastError();

	AtspiTextRange* pTextRange = atspi_text_get_string_at_offset(
		text_interface, cursor, GetAtspiTextGranularityFromTextGranularity(granularity), &m_data->last_error);
	if (!pTextRange)
		return std::unexpected(EObjectError::FAIL);
	defer(g_free(pTextRange));
	return GetTextRangeFromAtspiRange<AtspiTextRange, std::string>(*pTextRange);
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
