#include "ObjectAtspi.h"
#include "Event.h"
#include "Rect.h"

[[nodiscard]] IObject::EObjectType CObjectAtspi::GetType() {
	if (!m_accessible) return IObject::UNKNOWN;
	AtspiRole role = atspi_accessible_get_role(m_accessible, &m_lastError);
	return GetObjectTypeFromAtspiRole(role);
}

[[nodiscard]] bool CObjectAtspi::IsVisible() {
	return GetState() & IObject::VISIBLE;
}

[[nodiscard]] bool CObjectAtspi::IsEnabled() {
	return GetState() & IObject::ENABLED;
}

[[nodiscard]] unsigned int CObjectAtspi::GetState() {
	if (!m_accessible) return 0;
	AtspiStateSet* states = atspi_accessible_get_state_set(m_accessible);
	if (!states) return 0;
	return GetObjectStateFromAtspiStates(*states);
}

[[nodiscard]] bool CObjectAtspi::HasState(IObject::EObjectState state) {
	return GetState() & state;
}

bool CObjectAtspi::HandleEvent(const CEvent& event) {
	return false;
}

[[nodiscard]] IObject* CObjectAtspi::GetParent() {
	if (!m_accessible) return nullptr;
	if (m_parent) return m_parent;
	AtspiAccessible* parent = atspi_accessible_get_parent(m_accessible, &m_lastError);
	if (!parent) return nullptr;
	return new CObjectAtspi(parent);
}

[[nodiscard]] const std::vector<IObject*>& CObjectAtspi::GetChildren() {
	if (!m_accessible) return m_children;
	gint child_count = atspi_accessible_get_child_count(m_accessible, &m_lastError);
	if (child_count == 0) return m_children;

	for (gint i = 0; i < child_count; ++i) {
		AtspiAccessible* child = atspi_accessible_get_child_at_index(m_accessible, i, &m_lastError);
		if (!child) continue;
		CObjectAtspi* child_object = new CObjectAtspi(child);
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
	return "";
}

[[nodiscard]] std::string CObjectAtspi::GetName() {
	return "";
}

[[nodiscard]] std::string CObjectAtspi::GetDescription() {
	return "";
}

[[nodiscard]] std::string CObjectAtspi::GetText() {
	return "";
}

[[nodiscard]] IObject* CObjectAtspi::GetSelectedItem() {
	return nullptr;
}

[[nodiscard]] const std::vector<IObject*>& CObjectAtspi::GetItems() {
	return m_children;
}

[[nodiscard]] double CObjectAtspi::GetMinValue() {
	return 0.0;
}

[[nodiscard]] double CObjectAtspi::GetMaxValue() {
	return 0.0;
}

[[nodiscard]] double CObjectAtspi::GetCurrentValue() {
	return 0.0;
}
