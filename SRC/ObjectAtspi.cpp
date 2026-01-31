// AT-SPI object implementation.
#include "ObjectAtspi.h"

#include <utility>
#include "Event.h"
#include "Rect.h"
#include "Defer.h"

[[nodiscard]] auto GetDesktopObject() -> std::shared_ptr<IObject> {
	return std::make_shared<CObjectAtspi>(atspi_get_desktop(0));
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
	if (!m_accessible) return IObject::UNKNOWN;

	ResetLastError();
	AtspiRole role = atspi_accessible_get_role(m_accessible, &m_lastError);
	CacheReturn(m_type, GetObjectTypeFromAtspiRole(role));
}

[[nodiscard]] auto CObjectAtspi::IsVisible() const -> ObjectResult<bool> {
	return GetState().value() & IObject::VISIBLE;
}

[[nodiscard]] auto CObjectAtspi::IsEnabled() const -> ObjectResult<bool> {
	return GetState().value() & IObject::ENABLED;
}

[[nodiscard]] auto CObjectAtspi::GetState() const -> ObjectResult<unsigned long long> {
	ReturnCache(m_states);
	if (!m_accessible) return IObject::NO;
	AtspiStateSet* states = atspi_accessible_get_state_set(m_accessible);
	if (!states) return IObject::NO;
	defer(g_object_unref(states));
	CacheReturn(m_states, GetObjectStateFromAtspiStates(states));
}

[[nodiscard]] auto CObjectAtspi::HasState(IObject::EObjectState state) const -> ObjectResult<bool> {
	return GetState().value() & state;
}

[[nodiscard]] auto CObjectAtspi::GetParent() const -> ObjectResult<std::weak_ptr<IObject>> {
	ReturnCache(m_parent);

	if (!m_accessible) return std::weak_ptr<CObjectAtspi>();

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
	if (!m_accessible) return m_children.value();

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
	if (!m_accessible) return 0;

	ResetLastError();
	int children_count = atspi_accessible_get_child_count(m_accessible, &m_lastError);
	CacheReturn(m_childrenCount, children_count);
}

[[nodiscard]] auto CObjectAtspi::GetBounds() const -> ObjectResult<SRect> {
	return SRect{};
}

[[nodiscard]] auto CObjectAtspi::GetIndex() const -> ObjectResult<int> {
	ReturnCache(m_index);
	if (!m_accessible) return -1;

	ResetLastError();
	int index = atspi_accessible_get_index_in_parent(m_accessible, &m_lastError);
	CacheReturn(m_index, index);
}

[[nodiscard]] auto CObjectAtspi::GetApplicationName() const -> ObjectResult<std::string> {
	ReturnCache(m_applicationName);
	if (!m_accessible) return "Unknown";

	ResetLastError();
	CGlibString name(atspi_accessible_get_toolkit_name(m_accessible, &m_lastError));
	CacheReturn(m_applicationName, name);
}

[[nodiscard]] auto CObjectAtspi::GetName() const -> ObjectResult<std::string> {
	ReturnCache(m_name);
	if (!m_accessible) return "Unknown";

	ResetLastError();
	CGlibString name(atspi_accessible_get_name(m_accessible, &m_lastError));
	CacheReturn(m_name, name);
}

[[nodiscard]] auto CObjectAtspi::GetDescription() const -> ObjectResult<std::string> {
	ReturnCache(m_description);
	if (!m_accessible) return "";

	ResetLastError();

	CGlibString description(atspi_accessible_get_description(m_accessible, &m_lastError));

	CacheReturn(m_description, description);
}

[[nodiscard]] auto CObjectAtspi::GetCursor() const -> ObjectResult<int> {
	ReturnCache(m_cursor);
	if (!m_accessible) return 0;
	if (!m_textInterface) {
		m_textInterface = atspi_accessible_get_text_iface(m_accessible);
		if (!m_textInterface) CacheReturn(m_cursor, 0);
	}

	ResetLastError();

	CacheReturn(m_cursor, atspi_text_get_caret_offset(m_textInterface, &m_lastError));
}

[[nodiscard]] auto CObjectAtspi::GetText(int cursor, const ETextGranularity& granularity) const -> ObjectResult<STextRange> {
	STextRange text_range;
	if (!m_accessible) return text_range;
	if (!m_textInterface) {
		m_textInterface = atspi_accessible_get_text_iface(m_accessible);
		if (!m_textInterface) return text_range;
	}

	ResetLastError();

	AtspiTextRange* pTextRange = atspi_text_get_string_at_offset(m_textInterface, cursor, GetAtspiTextGranularityFromTextGranularity(granularity), &m_lastError);
	if (!pTextRange) return text_range;
	defer(g_free(pTextRange));
	CGlibString content(pTextRange->content);
	text_range.start = pTextRange->start_offset;
	text_range.end = pTextRange->end_offset;
	text_range.text = content;
	return text_range;
}

[[nodiscard]] auto CObjectAtspi::GetMinValue() const -> ObjectResult<double> {
	ReturnCache(m_minValue);
	if (!m_accessible) return 0.0;
	if (!m_valueInterface) {
		m_valueInterface = atspi_accessible_get_value_iface(m_accessible);
		if (!m_valueInterface) CacheReturn(m_minValue, 0.0);
	}

	ResetLastError();

	CacheReturn(m_minValue, atspi_value_get_minimum_value(m_valueInterface, &m_lastError));
}

[[nodiscard]] auto CObjectAtspi::GetMaxValue() const -> ObjectResult<double> {
	ReturnCache(m_maxValue);
	if (!m_accessible) return 0.0;
	if (!m_valueInterface) {
		m_valueInterface = atspi_accessible_get_value_iface(m_accessible);
		if (!m_valueInterface) CacheReturn(m_maxValue, 0.0);
	}

	ResetLastError();

	CacheReturn(m_maxValue, atspi_value_get_maximum_value(m_valueInterface, &m_lastError));
}

[[nodiscard]] auto CObjectAtspi::GetCurrentValue() const -> ObjectResult<double> {
	ReturnCache(m_currentValue);
	if (!m_accessible) return 0.0;
	if (!m_valueInterface) {
		m_valueInterface = atspi_accessible_get_value_iface(m_accessible);
		if (!m_valueInterface) CacheReturn(m_currentValue, 0.0);
	}

	ResetLastError();

	CacheReturn(m_currentValue, atspi_value_get_current_value(m_valueInterface, &m_lastError));
}

