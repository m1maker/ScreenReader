// AT-SPI object implementation.
module;
#include <Core/Cache.h>
#include <Core/Defer.h>
#include <Core/Text.h>
#include <atspi/atspi.h>
#include <expected>
#include <string>
#include <utility>
module Platforms.Linux.Object;
import Core.Rect;

CObjectAtspi::CObjectAtspi(AtspiAccessible* accessible, SObjectAtspiData* data, std::pmr::memory_resource* pool)
	: m_accessible(accessible), m_data(data), TObject(pool) {
	/*
		if (atspi_accessible_is_text(m_accessible))
			m_data->interfaces_mask |= EObjectInterfaceMaskSUPPORTS_TEXT;
		if (atspi_accessible_is_selection(m_accessible))
			m_data->interfaces_mask |= SUPPORTS_SELECTION;
		if (atspi_accessible_is_value(m_accessible))
			m_data->interfaces_mask |= SUPPORTS_VALUE;
	*/
}

void CObjectAtspi::do_UpdateCacheByEvent(EObjectEventType event) {
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
}

[[nodiscard]] auto CObjectAtspi::do_GetType() const -> ObjectResult<EObjectType> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);
	ReturnCache(type);

	m_data->ResetLastError();
	AtspiRole role = atspi_accessible_get_role(m_accessible, &m_data->last_error);
	CacheReturn(type, GetObjectTypeFromAtspiRole(role));
}

[[nodiscard]] auto CObjectAtspi::do_GetState() const -> ObjectResult<unsigned long long> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);
	ReturnCache(states);

	AtspiStateSet* state_set = atspi_accessible_get_state_set(m_accessible);
	if (!state_set)
		return std::unexpected(EObjectError::FAIL);
	defer(g_object_unref(state_set));
	CacheReturn(states, GetObjectStateFromAtspiStates(state_set));
}

[[nodiscard]] auto CObjectAtspi::do_GetParent() const -> ObjectResult<CObjectAtspi> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);
	ReturnCache(parent);

	m_data->ResetLastError();
	AtspiAccessible* native_parent = atspi_accessible_get_parent(m_accessible, &m_data->last_error);

	if (!native_parent)
		return std::unexpected(EObjectError::FAIL);

	auto parent_object =
		CObjectCache<AtspiAccessible, SObjectAtspiData>::GetInstance().GetOrCreate<CObjectAtspi>(native_parent);
	CacheReturn(parent, parent_object);
}

[[nodiscard]] auto CObjectAtspi::do_GetChildren() const -> ObjectResult<std::vector<CObjectAtspi>> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);
	ReturnCacheTransformed(
		children, [](const auto& vec) -> auto { return std::vector<CObjectAtspi>(vec.begin(), vec.end()); });

	auto children_to_return = std::pmr::vector<CObjectAtspi>(m_pool);

	m_data->ResetLastError();
	gint child_count = atspi_accessible_get_child_count(m_accessible, &m_data->last_error);

	for (gint i = 0; i < child_count; ++i) {
		m_data->ResetLastError();
		AtspiAccessible* child_native = atspi_accessible_get_child_at_index(m_accessible, i, &m_data->last_error);
		if (!child_native)
			continue;

		auto child_object =
			CObjectCache<AtspiAccessible, SObjectAtspiData>::GetInstance().GetOrCreate<CObjectAtspi>(child_native);

		children_to_return.push_back(child_object);
	}

	CacheReturnTransformed(children, children_to_return, [](const auto& vec) -> auto {
		return std::vector<CObjectAtspi>(vec.begin(), vec.end());
	});
}

[[nodiscard]] auto CObjectAtspi::do_GetChildAt(int index) const -> ObjectResult<CObjectAtspi> {
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

[[nodiscard]] auto CObjectAtspi::do_GetChildrenCount() const -> ObjectResult<int> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);
	ReturnCache(children_count);

	m_data->ResetLastError();
	int child_count = atspi_accessible_get_child_count(m_accessible, &m_data->last_error);
	CacheReturn(children_count, child_count);
}

[[nodiscard]] auto CObjectAtspi::do_GetBounds() const -> ObjectResult<SRect> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);
	return std::unexpected(EObjectError::NOT_SUPPORTED);
}

[[nodiscard]] auto CObjectAtspi::do_GetIndex() const -> ObjectResult<int> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);
	ReturnCache(index);

	m_data->ResetLastError();
	int idx = atspi_accessible_get_index_in_parent(m_accessible, &m_data->last_error);
	CacheReturn(index, idx);
}

[[nodiscard]] auto CObjectAtspi::do_GetApplicationName() const -> ObjectResult<std::string> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);
	ReturnCache(application_name);

	m_data->ResetLastError();
	CGlibString name(atspi_accessible_get_toolkit_name(m_accessible, &m_data->last_error));
	CacheReturn(application_name, name);
}

[[nodiscard]] auto CObjectAtspi::do_GetName() const -> ObjectResult<std::string> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);

	ReturnCache(name);

	m_data->ResetLastError();
	CGlibString obj_name(atspi_accessible_get_name(m_accessible, &m_data->last_error));
	CacheReturn(name, obj_name);
}

[[nodiscard]] auto CObjectAtspi::do_GetDescription() const -> ObjectResult<std::string> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);
	ReturnCache(description);

	m_data->ResetLastError();

	CGlibString desc(atspi_accessible_get_description(m_accessible, &m_data->last_error));

	CacheReturn(description, desc);
}

[[nodiscard]] auto CObjectAtspi::do_GetCursor() const -> ObjectResult<int> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);
	ReturnCache(cursor);

	SAtspiIface<AtspiText> text_interface(atspi_accessible_get_text_iface(m_accessible));
	if (!text_interface.pointer)
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	m_data->ResetLastError();

	CacheReturn(cursor, atspi_text_get_caret_offset(text_interface, &m_data->last_error));
}

[[nodiscard]] auto CObjectAtspi::do_GetText(int cursor, const ETextGranularity& granularity) const
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

[[nodiscard]] auto CObjectAtspi::do_GetSelectedRanges() const -> ObjectResult<std::vector<STextRange<void>>> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);
	SAtspiIface<AtspiText> text_interface(atspi_accessible_get_text_iface(m_accessible));
	if (!text_interface.pointer)
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	m_data->ResetLastError();

	std::vector<STextRange<void>> text_ranges;
	gint selection_count = atspi_text_get_n_selections(text_interface, &m_data->last_error);
	for (gint i = 0; i < selection_count; ++i) {
		m_data->ResetLastError();

		AtspiRange* pRange = atspi_text_get_selection(text_interface, i, &m_data->last_error);
		if (!pRange)
			continue;

		auto text_range = GetTextRangeFromAtspiRange<AtspiRange, void>(*pRange);
		text_ranges.emplace_back(text_range);
		g_free(pRange);
	}

	return text_ranges;
}

[[nodiscard]] auto CObjectAtspi::do_GetSelectedItems() const -> ObjectResult<std::vector<CObjectAtspi>> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);
	SAtspiIface<AtspiSelection> selection_interface(atspi_accessible_get_selection_iface(m_accessible));
	if (!selection_interface.pointer)
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	auto children = std::vector<CObjectAtspi>();

	m_data->ResetLastError();

	gint selection_count = atspi_selection_get_n_selected_children(selection_interface, &m_data->last_error);
	for (gint i = 0; i < selection_count; ++i) {
		m_data->ResetLastError();
		AtspiAccessible* child_native = atspi_selection_get_selected_child(selection_interface, i, &m_data->last_error);
		if (!child_native)
			continue;

		auto child_object =
			CObjectCache<AtspiAccessible, SObjectAtspiData>::GetInstance().GetOrCreate<CObjectAtspi>(child_native);

		children.push_back(child_object);
	}

	return children;
}

[[nodiscard]] auto CObjectAtspi::do_GetMinValue() const -> ObjectResult<double> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);
	ReturnCache(min_value);

	SAtspiIface<AtspiValue> value_interface(atspi_accessible_get_value_iface(m_accessible));
	if (!value_interface.pointer)
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	m_data->ResetLastError();

	CacheReturn(min_value, atspi_value_get_minimum_value(value_interface, &m_data->last_error));
}

[[nodiscard]] auto CObjectAtspi::do_GetMaxValue() const -> ObjectResult<double> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);
	ReturnCache(max_value);

	SAtspiIface<AtspiValue> value_interface(atspi_accessible_get_value_iface(m_accessible));
	if (!value_interface.pointer)
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	m_data->ResetLastError();

	CacheReturn(max_value, atspi_value_get_maximum_value(value_interface, &m_data->last_error));
}

[[nodiscard]] auto CObjectAtspi::do_GetCurrentValue() const -> ObjectResult<double> {
	if (!IsValid()) [[unlikely]]
		return std::unexpected(EObjectError::DEFUNCT);
	ReturnCache(current_value);

	SAtspiIface<AtspiValue> value_interface(atspi_accessible_get_value_iface(m_accessible));
	if (!value_interface.pointer)
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	m_data->ResetLastError();

	CacheReturn(current_value, atspi_value_get_current_value(value_interface, &m_data->last_error));
}
