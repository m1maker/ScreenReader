#include "ObjectAtspi.h"
#include "Event.h"
#include "Rect.h"

[[nodiscard]] IObject::EObjectType CObjectAtspi::GetType() {
	if (!m_accessible) return IObject::UNKNOWN;
	AtspiRole role = atspi_accessible_get_role(&*m_accessible, &m_lastError);
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
	AtspiStateSet* states = atspi_accessible_get_state_set(&*m_accessible);
	if (!states) return 0;
	return GetObjectStateFromAtspiStates(*states);
}

[[nodiscard]] bool CObjectAtspi::HasState(IObject::EObjectState state) {
	return GetState() & state;
}

[[nodiscard]] std::weak_ptr<IObject> CObjectAtspi::GetParent() {
	if (!m_accessible) return std::make_shared<CObjectAtspi>(nullptr);
	AtspiAccessible* parent = atspi_accessible_get_parent(&*m_accessible, &m_lastError);
	if (!parent) return std::make_shared<CObjectAtspi>(nullptr);
	return std::make_shared<CObjectAtspi>(parent);
}

[[nodiscard]] const std::vector<std::shared_ptr<IObject>>& CObjectAtspi::GetChildren() {
	if (!m_accessible) return m_children;
	gint child_count = atspi_accessible_get_child_count(&*m_accessible, &m_lastError);
	if (child_count == 0) return m_children;

	for (gint i = 0; i < child_count; ++i) {
		AtspiAccessible* child = atspi_accessible_get_child_at_index(&*m_accessible, i, &m_lastError);
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
	gchar* name = atspi_accessible_get_toolkit_name(&*m_accessible, &m_lastError);
	if (!name || !*name) return "Unknown";
	return std::string(name);
}

[[nodiscard]] std::string CObjectAtspi::GetName() {
	if (!m_accessible) return "Unknown";
	gchar* name = atspi_accessible_get_name(&*m_accessible, &m_lastError);
	if (!name || !*name) return "Unknown";
	return std::string(name);
}

[[nodiscard]] std::string CObjectAtspi::GetDescription() {
	if (!m_accessible) return "";
	gchar* description = atspi_accessible_get_description(&*m_accessible, &m_lastError);
	if (!description || !*description) return "";
	return std::string(description);
}

[[nodiscard]] std::string CObjectAtspi::GetText() {
	return ""; /// I don't know, for what it will. Either editable text or static text.
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
