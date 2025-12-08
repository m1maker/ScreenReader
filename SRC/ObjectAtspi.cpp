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

[[nodiscard]] auto CObjectAtspi::GetType() const -> IObject::EObjectType {
	ReturnCache(m_type);
	if (!m_accessible) return IObject::UNKNOWN;

	ResetLastError();
	AtspiRole role = atspi_accessible_get_role(m_accessible, &m_lastError);
	CacheReturn(m_type, GetObjectTypeFromAtspiRole(role));
}

[[nodiscard]] auto CObjectAtspi::IsVisible() const -> bool {
	return GetState() & IObject::VISIBLE;
}

[[nodiscard]] auto CObjectAtspi::IsEnabled() const -> bool {
	return GetState() & IObject::ENABLED;
}

[[nodiscard]] auto CObjectAtspi::GetState() const -> unsigned long long {
	ReturnCache(m_states);
	if (!m_accessible) return IObject::NO;
	AtspiStateSet* states = atspi_accessible_get_state_set(m_accessible);
	if (!states) return IObject::NO;
	defer(g_object_unref(states));
	CacheReturn(m_states, GetObjectStateFromAtspiStates(states));
}

[[nodiscard]] auto CObjectAtspi::HasState(IObject::EObjectState state) const -> bool {
	return GetState() & state;
}

[[nodiscard]] auto CObjectAtspi::GetParent() const -> std::weak_ptr<IObject> {
	ReturnCache(m_parent);

	if (!m_accessible) return std::weak_ptr<CObjectAtspi>();

	ResetLastError();
	AtspiAccessible* native_parent = atspi_accessible_get_parent(m_accessible, &m_lastError);

	if (!native_parent) return std::weak_ptr<CObjectAtspi>();

	auto parent_object = g_objectAtspiCache.GetOrCreate(native_parent);

	m_strongParentCache = parent_object;

	CacheReturn(m_parent, m_strongParentCache);
}

[[nodiscard]] auto CObjectAtspi::GetChildren() const -> const std::vector<std::shared_ptr<IObject>>& {
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

		auto child_object = g_objectAtspiCache.GetOrCreate(child_native);

		children.push_back(child_object);
	}

	CacheReturn(m_children, children);
}

[[nodiscard]] auto CObjectAtspi::GetChildrenCount() const -> int {
	ReturnCache(m_childrenCount);
	if (!m_accessible) return 0;

	ResetLastError();
	int children_count = atspi_accessible_get_child_count(m_accessible, &m_lastError);
	CacheReturn(m_childrenCount, children_count);
}

[[nodiscard]] auto CObjectAtspi::GetBounds() const -> SRect {
	return SRect{};
}

[[nodiscard]] auto CObjectAtspi::GetIndex() const -> int {
	ReturnCache(m_index);
	if (!m_accessible) return -1;

	ResetLastError();
	int index = atspi_accessible_get_index_in_parent(m_accessible, &m_lastError);
	CacheReturn(m_index, index);
}

[[nodiscard]] auto CObjectAtspi::GetApplicationName() const -> std::string {
	ReturnCache(m_applicationName);
	if (!m_accessible) return "Unknown";

	ResetLastError();
	CGlibString name(atspi_accessible_get_toolkit_name(m_accessible, &m_lastError));
	CacheReturn(m_applicationName, name);
}

[[nodiscard]] auto CObjectAtspi::GetName() const -> std::string {
	ReturnCache(m_name);
	if (!m_accessible) return "Unknown";

	bool name_found = false;

	ResetLastError();
	CGlibString name(atspi_accessible_get_name(m_accessible, &m_lastError));

	std::vector<AtspiRelation> relations = GetRelations();
	if (relations.empty()) CacheReturn(m_name, name);
	for (AtspiRelation& relation : relations) {
		if (atspi_relation_get_relation_type(&relation) == ATSPI_RELATION_LABELLED_BY) {
			ResetLastError();

			name = CGlibString(atspi_accessible_get_name(atspi_relation_get_target(&relation, 0), &m_lastError));
			name_found = true;
			break;
		}
	}

	CacheReturn(m_name, name);
}

[[nodiscard]] auto CObjectAtspi::GetDescription() const -> std::string {
	ReturnCache(m_description);
	if (!m_accessible) return "";

	bool description_found = false;
	ResetLastError();

	CGlibString description(atspi_accessible_get_description(m_accessible, &m_lastError));

	std::vector<AtspiRelation> relations = GetRelations();
	if (relations.empty()) CacheReturn(m_description, description);
	for (AtspiRelation& relation : relations) {
		if (atspi_relation_get_relation_type(&relation) == ATSPI_RELATION_DESCRIBED_BY) {
			ResetLastError();

			description = CGlibString(atspi_accessible_get_description(atspi_relation_get_target(&relation, 0), &m_lastError));
			description_found = true;
			break;
		}
	}

	CacheReturn(m_description, description);
}

[[nodiscard]] auto CObjectAtspi::GetCursor() const -> int {
	ReturnCache(m_cursor);
	if (!m_accessible) return 0;
	if (!m_textInterface) {
		m_textInterface = atspi_accessible_get_text_iface(m_accessible);
		if (!m_textInterface) CacheReturn(m_cursor, 0);
	}

	ResetLastError();

	CacheReturn(m_cursor, atspi_text_get_caret_offset(m_textInterface, &m_lastError));
}

[[nodiscard]] auto CObjectAtspi::GetText(int cursor, const ETextGranularity& granularity) const -> STextRange {
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

[[nodiscard]] auto CObjectAtspi::GetMinValue() const -> double {
	ReturnCache(m_minValue);
	if (!m_accessible) return 0.0;
	if (!m_valueInterface) {
		m_valueInterface = atspi_accessible_get_value_iface(m_accessible);
		if (!m_valueInterface) CacheReturn(m_minValue, 0.0);
	}

	ResetLastError();

	CacheReturn(m_minValue, atspi_value_get_minimum_value(m_valueInterface, &m_lastError));
}

[[nodiscard]] auto CObjectAtspi::GetMaxValue() const -> double {
	ReturnCache(m_maxValue);
	if (!m_accessible) return 0.0;
	if (!m_valueInterface) {
		m_valueInterface = atspi_accessible_get_value_iface(m_accessible);
		if (!m_valueInterface) CacheReturn(m_maxValue, 0.0);
	}

	ResetLastError();

	CacheReturn(m_maxValue, atspi_value_get_maximum_value(m_valueInterface, &m_lastError));
}

[[nodiscard]] auto CObjectAtspi::GetCurrentValue() const -> double {
	ReturnCache(m_currentValue);
	if (!m_accessible) return 0.0;
	if (!m_valueInterface) {
		m_valueInterface = atspi_accessible_get_value_iface(m_accessible);
		if (!m_valueInterface) CacheReturn(m_currentValue, 0.0);
	}

	ResetLastError();

	CacheReturn(m_currentValue, atspi_value_get_current_value(m_valueInterface, &m_lastError));
}

[[nodiscard]] auto CObjectAtspiCache::GetOrCreate(AtspiAccessible* accessible) -> std::shared_ptr<CObjectAtspi> {
	if (!accessible) return nullptr;

	std::lock_guard<std::mutex> lock(m_mutex);

	auto it = m_cache.find(accessible);
	if (it != m_cache.end()) {
		if (auto existing = it->second.lock()) {
			g_object_unref(accessible); 
			return existing;
		}
		else {
			m_cache.erase(it);
		}
	}

	auto new_object = std::make_shared<CObjectAtspi>(accessible);

	m_cache[accessible] = new_object;

	return new_object;
}

void CObjectAtspiCache::Remove(AtspiAccessible* accessible) {
	std::lock_guard<std::mutex> lock(m_mutex);
	m_cache.erase(accessible);
}

