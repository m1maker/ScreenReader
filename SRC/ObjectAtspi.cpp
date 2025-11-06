// AT-SPI object implementation.
#include "ObjectAtspi.h"
#include "Event.h"
#include "Rect.h"

[[nodiscard]] std::shared_ptr<IObject> GetDesktopObject() {
	return std::make_shared<CObjectAtspi>(atspi_get_desktop(0));
}

[[nodiscard]] std::vector<AtspiRelation> CObjectAtspi::GetRelations() {
	if (m_relations) g_array_free(m_relations, TRUE);
	if (!m_accessible) return{};
	ResetLastError();

	m_relations = atspi_accessible_get_relation_set(m_accessible, &m_lastError);
	if (!m_relations) return {};

	std::vector<AtspiRelation> relations;
	relations.reserve(m_relations->len);
	for (gint i = 0; i < m_relations->len; ++i) {
		AtspiRelation* relation = g_array_index(m_relations, AtspiRelation*, i);
		relations.push_back(*relation);
	}

	return relations;
}

[[nodiscard]] IObject::EObjectType CObjectAtspi::GetType() {
	if (!m_accessible) return IObject::UNKNOWN;

	ResetLastError();
	AtspiRole role = atspi_accessible_get_role(m_accessible, &m_lastError);
	if (role == ATSPI_ROLE_PASSWORD_TEXT) m_states = SECURE;
	else if (m_states != NO) m_states = NO;
	return GetObjectTypeFromAtspiRole(role);
}

[[nodiscard]] bool CObjectAtspi::IsVisible() {
	return GetState() & IObject::VISIBLE;
}

[[nodiscard]] bool CObjectAtspi::IsEnabled() {
	return GetState() & IObject::ENABLED;
}

[[nodiscard]] IObject::EObjectState CObjectAtspi::GetState() {
	if (!m_accessible) return IObject::NO;
	AtspiStateSet* states = atspi_accessible_get_state_set(m_accessible);
	if (!states) return IObject::NO;
	return static_cast<EObjectState>(GetObjectStateFromAtspiStates(states) | m_states);
}

[[nodiscard]] bool CObjectAtspi::HasState(IObject::EObjectState state) {
	return GetState() & state;
}

[[nodiscard]] std::weak_ptr<IObject> CObjectAtspi::GetParent() {
	if (!m_accessible) return std::weak_ptr<CObjectAtspi>();

	ResetLastError();
	AtspiAccessible* parent = atspi_accessible_get_parent(m_accessible, &m_lastError);
	if (!parent) return std::weak_ptr<CObjectAtspi>();
	return std::make_shared<CObjectAtspi>(parent);
}

[[nodiscard]] const std::vector<std::shared_ptr<IObject>>& CObjectAtspi::GetChildren() {
	if (!m_accessible) return m_children;

	ResetLastError();
	gint child_count = atspi_accessible_get_child_count(m_accessible, &m_lastError);
	if (child_count == 0) return m_children;

	for (gint i = 0; i < child_count; ++i) {
		ResetLastError();

		AtspiAccessible* child = atspi_accessible_get_child_at_index(m_accessible, i, &m_lastError);
		if (!child) continue;
		std::shared_ptr<CObjectAtspi> child_object = std::make_shared<CObjectAtspi>(child);
		m_children.push_back(child_object);
	}

	return m_children;
}

[[nodiscard]] SRect CObjectAtspi::GetBounds() {
	return SRect{};
}

[[nodiscard]] int CObjectAtspi::GetTabIndex() {
	return -1;
}

[[nodiscard]] std::string CObjectAtspi::GetApplicationName() {
	if (!m_accessible) return "Unknown";

	ResetLastError();
	CGlibString name(atspi_accessible_get_toolkit_name(m_accessible, &m_lastError));
	return name;
}

[[nodiscard]] std::string CObjectAtspi::GetName() {
	if (!m_accessible) return "Unknown";

	bool name_found = false;

	ResetLastError();
	CGlibString name(atspi_accessible_get_name(m_accessible, &m_lastError));

	std::vector<AtspiRelation> relations = GetRelations();
	if (relations.empty()) return name;
	for (AtspiRelation& relation : relations) {
		if (atspi_relation_get_relation_type(&relation) == ATSPI_RELATION_LABELLED_BY) {
			ResetLastError();

			name = CGlibString(atspi_accessible_get_name(atspi_relation_get_target(&relation, 0), &m_lastError));
			name_found = true;
			break;
		}
	}

	return name;
}

[[nodiscard]] std::string CObjectAtspi::GetDescription() {
	if (!m_accessible) return "";

	bool description_found = false;
	ResetLastError();

	CGlibString description(atspi_accessible_get_description(m_accessible, &m_lastError));

	std::vector<AtspiRelation> relations = GetRelations();
	if (relations.empty()) return description;
	for (AtspiRelation& relation : relations) {
		if (atspi_relation_get_relation_type(&relation) == ATSPI_RELATION_DESCRIBED_BY) {
			ResetLastError();

			description = CGlibString(atspi_accessible_get_description(atspi_relation_get_target(&relation, 0), &m_lastError));
			description_found = true;
			break;
		}
	}

	return description;
}

[[nodiscard]] int CObjectAtspi::GetCursor() {
	if (!m_accessible) return 0;
	if (!m_textInterface) {
		m_textInterface = atspi_accessible_get_text_iface(m_accessible);
		if (!m_textInterface) return 0;
	}

	ResetLastError();

	return atspi_text_get_caret_offset(m_textInterface, &m_lastError);
}

[[nodiscard]] std::string CObjectAtspi::GetText(bool at_cursor) {
	if (!m_accessible) return "";
	if (!m_textInterface) {
		m_textInterface = atspi_accessible_get_text_iface(m_accessible);
		if (!m_textInterface) return "";
	}

	ResetLastError();

	gint character_count = atspi_text_get_character_count(m_textInterface, &m_lastError);
	if (character_count <=0) return "";

	ResetLastError();

	CGlibString text(nullptr);
	if (at_cursor) {
		int cursor = GetCursor();
		text = CGlibString(atspi_text_get_text(m_textInterface, cursor, cursor + 1, &m_lastError));
	}
	else 		text = CGlibString(atspi_text_get_text(m_textInterface, 0, character_count, &m_lastError));

	return text;
}

[[nodiscard]] double CObjectAtspi::GetMinValue() {
	if (!m_accessible) return 0.0;
	if (!m_valueInterface) {
		m_valueInterface = atspi_accessible_get_value_iface(m_accessible);
		if (!m_valueInterface) return 0.0;
	}

	ResetLastError();

	return atspi_value_get_minimum_value(m_valueInterface, &m_lastError);
}

[[nodiscard]] double CObjectAtspi::GetMaxValue() {
	if (!m_accessible) return 0.0;
	if (!m_valueInterface) {
		m_valueInterface = atspi_accessible_get_value_iface(m_accessible);
		if (!m_valueInterface) return 0.0;
	}

	ResetLastError();

	return atspi_value_get_maximum_value(m_valueInterface, &m_lastError);
}

[[nodiscard]] double CObjectAtspi::GetCurrentValue() {
	if (!m_accessible) return 0.0;
	if (!m_valueInterface) {
		m_valueInterface = atspi_accessible_get_value_iface(m_accessible);
		if (!m_valueInterface) return 0.0;
	}

	ResetLastError();

	return atspi_value_get_current_value(m_valueInterface, &m_lastError);
}
