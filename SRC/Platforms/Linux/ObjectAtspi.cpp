// AT-SPI object implementation.
#include "ObjectAtspi.h"

#include <utility>
#include <Core/Event.h>
#include <Core/Rect.h>
#include <Core/Defer.h>

[[nodiscard]] auto GetDesktopObject() -> std::shared_ptr<IObject> {
	return std::make_shared<CObjectAtspi>(atspi_get_desktop(0));
}

void CObjectAtspi::UpdateCacheByEvent(const CEvent::EEventType& event) {
	switch (event) {
		case CEvent::VALUE_CHANGED:
			Cache(m_minValue, std::nullopt);
			Cache(m_maxValue, std::nullopt);
			Cache(m_currentValue, std::nullopt);
			break;
		//case CEvent::SELECTION_CHANGED:
		case CEvent::STATE_CHANGED:
		case CEvent::VISIBILITY_CHANGED:
		case CEvent::ENABLED_CHANGED:
			Cache(m_states, std::nullopt);
			break;
		//case CEvent::TEXT_CHANGED:
			//Cache(m_text, std::nullopt);
			//break;
		case CEvent::CURSOR_MOVED:
			Cache(m_cursor, std::nullopt);
			break;
		case CEvent::CHILD_ADDED:
		case CEvent::CHILD_REMOVED:
			Cache(m_children, std::nullopt);
			break;
		case CEvent::PARENT_UPDATED:
			Cache(m_parent, std::nullopt);
			break;
		default: break;
	}
}

[[nodiscard]] auto CObjectAtspi::GetRelations() const -> std::vector<AtspiRelation> {
	if (m_relations) g_array_free(m_relations, TRUE);
	if (!m_accessible) return{};
	ResetLastError();

	m_relations = atspi_accessible_get_relation_set(m_accessible, &m_lastError);
	if (!m_relations) return {};

	std::vector<AtspiRelation> relations;
	relations.reserve(m_relations->len);
	for (gint i = 0; std::cmp_less(i , m_relations->len); ++i) {
		AtspiRelation* relation = g_array_index(m_relations, AtspiRelation*, i);
		relations.push_back(*relation);
	}

	return relations;
}

[[nodiscard]] auto CObjectAtspi::GetType() const -> ObjectResult<IObject::EObjectType> {
	ReturnCache(m_type);
	if (!m_accessible) return std::unexpected(IObject::DEFUNCT);

	ResetLastError();
	AtspiRole role = atspi_accessible_get_role(m_accessible, &m_lastError);
	CacheReturn(m_type, GetObjectTypeFromAtspiRole(role));
}

[[nodiscard]] auto CObjectAtspi::IsVisible() const -> ObjectResult<bool> {
	return GetState().value_or(false) & IObject::VISIBLE;
}

[[nodiscard]] auto CObjectAtspi::IsEnabled() const -> ObjectResult<bool> {
	return GetState().value_or(false) & IObject::ENABLED;
}

[[nodiscard]] auto CObjectAtspi::GetState() const -> ObjectResult<unsigned long long> {
	ReturnCache(m_states);
	if (!m_accessible) return std::unexpected(IObject::DEFUNCT);
	AtspiStateSet* states = atspi_accessible_get_state_set(m_accessible);
	if (!states) return std::unexpected(IObject::FAIL);
	defer(g_object_unref(states));
	CacheReturn(m_states, GetObjectStateFromAtspiStates(states));
}

[[nodiscard]] auto CObjectAtspi::HasState(IObject::EObjectState state) const -> ObjectResult<bool> {
	return GetState().value_or(0) & state;
}

[[nodiscard]] auto CObjectAtspi::GetParent() const -> ObjectResult<std::weak_ptr<IObject>> {
	ReturnCache(m_parent);

	if (!m_accessible) return std::unexpected(IObject::DEFUNCT);

	ResetLastError();
	AtspiAccessible* native_parent = atspi_accessible_get_parent(m_accessible, &m_lastError);

	if (!native_parent) return std::weak_ptr<CObjectAtspi>();

	auto parent_object = g_objectCache(AtspiAccessible, CObjectAtspi).GetOrCreate(native_parent);

	m_strongParentCache = parent_object;

	CacheReturn(m_parent, m_strongParentCache);
}

[[nodiscard]] auto CObjectAtspi::GetChildren() const -> ObjectResult<const std::vector<std::shared_ptr<IObject>>> {
	ReturnCache(m_children);
	auto children = std::vector<std::shared_ptr<IObject>>();
	if (!m_accessible) return std::unexpected(IObject::DEFUNCT);

	ResetLastError();
	gint child_count = atspi_accessible_get_child_count(m_accessible, &m_lastError);
	if (child_count == 0) CacheReturn(m_children, children);

	for (gint i = 0; i < child_count; ++i) {
		ResetLastError();
		AtspiAccessible* child_native = atspi_accessible_get_child_at_index(m_accessible, i, &m_lastError);
		if (!child_native) continue;

		auto child_object = g_objectCache(AtspiAccessible, CObjectAtspi).GetOrCreate(child_native);

		children.push_back(child_object);
	}

	CacheReturn(m_children, children);
}

[[nodiscard]] auto CObjectAtspi::GetChildrenCount() const -> ObjectResult<int> {
	ReturnCache(m_childrenCount);
	if (!m_accessible) return std::unexpected(IObject::DEFUNCT);

	ResetLastError();
	int children_count = atspi_accessible_get_child_count(m_accessible, &m_lastError);
	CacheReturn(m_childrenCount, children_count);
}

[[nodiscard]] auto CObjectAtspi::GetBounds() const -> ObjectResult<SRect> {
	return std::unexpected(IObject::NOT_SUPPORTED);
}

[[nodiscard]] auto CObjectAtspi::GetIndex() const -> ObjectResult<int> {
	ReturnCache(m_index);
	if (!m_accessible) return std::unexpected(IObject::DEFUNCT);

	ResetLastError();
	int index = atspi_accessible_get_index_in_parent(m_accessible, &m_lastError);
	CacheReturn(m_index, index);
}

[[nodiscard]] auto CObjectAtspi::GetApplicationName() const -> ObjectResult<std::string> {
	ReturnCache(m_applicationName);
	if (!m_accessible) return std::unexpected(IObject::DEFUNCT);

	ResetLastError();
	CGlibString name(atspi_accessible_get_toolkit_name(m_accessible, &m_lastError));
	CacheReturn(m_applicationName, name);
}

[[nodiscard]] auto CObjectAtspi::GetName() const -> ObjectResult<std::string> {
	ReturnCache(m_name);
	if (!m_accessible) return std::unexpected(IObject::DEFUNCT);

	ResetLastError();
	CGlibString name(atspi_accessible_get_name(m_accessible, &m_lastError));
	CacheReturn(m_name, name);
}

[[nodiscard]] auto CObjectAtspi::GetDescription() const -> ObjectResult<std::string> {
	ReturnCache(m_description);
	if (!m_accessible) return std::unexpected(IObject::DEFUNCT);

	ResetLastError();

	CGlibString description(atspi_accessible_get_description(m_accessible, &m_lastError));

	CacheReturn(m_description, description);
}

[[nodiscard]] auto CObjectAtspi::GetCursor() const -> ObjectResult<int> {
	ReturnCache(m_cursor);
	if (!m_accessible) return std::unexpected(IObject::DEFUNCT);
	if (!m_textInterface) {
		m_textInterface = atspi_accessible_get_text_iface(m_accessible);
		if (!m_textInterface) return std::unexpected(IObject::NOT_SUPPORTED);
	}

	ResetLastError();

	CacheReturn(m_cursor, atspi_text_get_caret_offset(m_textInterface, &m_lastError));
}

[[nodiscard]] auto CObjectAtspi::GetText(int cursor, const ETextGranularity& granularity) const -> ObjectResult<STextRange> {
	if (!m_accessible) return std::unexpected(IObject::DEFUNCT);
	if (!m_textInterface) {
		m_textInterface = atspi_accessible_get_text_iface(m_accessible);
		if (!m_textInterface) return std::unexpected(IObject::NOT_SUPPORTED);
	}

	ResetLastError();

	AtspiTextRange* pTextRange = atspi_text_get_string_at_offset(m_textInterface, cursor, GetAtspiTextGranularityFromTextGranularity(granularity), &m_lastError);
	if (!pTextRange) return std::unexpected(IObject::FAIL);
	defer(g_free(pTextRange));
	return GetTextRangeFromAtspiRange(*pTextRange);
}

[[nodiscard]] auto CObjectAtspi::GetSelectedRanges() const -> ObjectResult<std::vector<STextRange>> {
	if (!m_accessible) return std::unexpected(IObject::DEFUNCT);
	if (!m_textInterface) {
		m_textInterface = atspi_accessible_get_text_iface(m_accessible);
		if (!m_textInterface) return std::unexpected(IObject::NOT_SUPPORTED);
	}

	ResetLastError();

	std::vector<STextRange> text_ranges;
	gint selection_count = atspi_text_get_n_selections(m_textInterface, &m_lastError);
	for (gint i = 0; i < selection_count; ++i) {
		ResetLastError();

		AtspiRange* pRange = atspi_text_get_selection(m_textInterface, i, &m_lastError);
		if (!pRange) continue;

		ResetLastError();
		CGlibString text(atspi_text_get_text(m_textInterface, pRange->start_offset, pRange->end_offset, &m_lastError));
		auto text_range = GetTextRangeFromAtspiRange(*pRange);
		text_range.text = text;
		text_ranges.emplace_back(text_range);
		g_free(pRange);
	}

	return text_ranges;
}

[[nodiscard]] auto CObjectAtspi::GetSelectedItems() const -> ObjectResult<std::vector<std::shared_ptr<IObject>>> {
	if (!m_accessible) return std::unexpected(IObject::DEFUNCT);
	if (!m_selectionInterface) {
		m_selectionInterface = atspi_accessible_get_selection_iface(m_accessible);
		if (!m_selectionInterface) return std::unexpected(IObject::NOT_SUPPORTED);
	}

	auto children = std::vector<std::shared_ptr<IObject>>();

	ResetLastError();

	gint selection_count = atspi_selection_get_n_selected_children(m_selectionInterface, &m_lastError);
	for (gint i = 0; i < selection_count; ++i) {
		ResetLastError();
		AtspiAccessible* child_native = atspi_selection_get_selected_child(m_selectionInterface, i, &m_lastError);
		if (!child_native) continue;

		auto child_object = g_objectCache(AtspiAccessible, CObjectAtspi).GetOrCreate(child_native);

		children.push_back(child_object);
	}

	return children;
}

[[nodiscard]] auto CObjectAtspi::GetMinValue() const -> ObjectResult<double> {
	ReturnCache(m_minValue);
	if (!m_accessible) return std::unexpected(IObject::DEFUNCT);
	if (!m_valueInterface) {
		m_valueInterface = atspi_accessible_get_value_iface(m_accessible);
		if (!m_valueInterface) return std::unexpected(IObject::NOT_SUPPORTED);
	}

	ResetLastError();

	CacheReturn(m_minValue, atspi_value_get_minimum_value(m_valueInterface, &m_lastError));
}

[[nodiscard]] auto CObjectAtspi::GetMaxValue() const -> ObjectResult<double> {
	ReturnCache(m_maxValue);
	if (!m_accessible) return std::unexpected(IObject::DEFUNCT);
	if (!m_valueInterface) {
		m_valueInterface = atspi_accessible_get_value_iface(m_accessible);
		if (!m_valueInterface) return std::unexpected(IObject::NOT_SUPPORTED);
	}

	ResetLastError();

	CacheReturn(m_maxValue, atspi_value_get_maximum_value(m_valueInterface, &m_lastError));
}

[[nodiscard]] auto CObjectAtspi::GetCurrentValue() const -> ObjectResult<double> {
	ReturnCache(m_currentValue);
	if (!m_accessible) return std::unexpected(IObject::DEFUNCT);
	if (!m_valueInterface) {
		m_valueInterface = atspi_accessible_get_value_iface(m_accessible);
		if (!m_valueInterface) return std::unexpected(IObject::NOT_SUPPORTED);
	}

	ResetLastError();

	CacheReturn(m_currentValue, atspi_value_get_current_value(m_valueInterface, &m_lastError));
}

