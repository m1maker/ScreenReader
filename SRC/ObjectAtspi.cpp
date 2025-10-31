#include "ObjectAtspi.h"
#include "Event.h"
#include "Rect.h"

[[nodiscard]] IObject::EObjectType CObjectAtspi::GetType() {
	AtspiRole role = atspi_accessible_get_role(m_accessible, &m_lastError);
	return GetObjectTypeFromAtspiRole(role);
}

[[nodiscard]] bool CObjectAtspi::IsVisible() {
	return false;
}

[[nodiscard]] bool CObjectAtspi::IsEnabled() {
	return false;
}

[[nodiscard]] unsigned int CObjectAtspi::GetState() {
	return 0;
}

[[nodiscard]] bool CObjectAtspi::HasState(IObject::EObjectState state) {
	return false;
}

bool CObjectAtspi::HandleEvent(const CEvent& event) {
	return false;
}

[[nodiscard]] IObject* CObjectAtspi::GetParent() {
	return nullptr;
}

[[nodiscard]] const std::vector<IObject*>& CObjectAtspi::GetChildren() {
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
