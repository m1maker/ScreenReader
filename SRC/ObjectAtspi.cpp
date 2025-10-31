#include "ObjectAtspi.h"
#include "Event.h"
#include "Rect.h"

[[nodiscard]] IObject::EObjectType CObjectAtspi::GetType() const {
	return IObject::UNKNOWN;
}

[[nodiscard]] bool CObjectAtspi::IsVisible() const {
	return false;
}

[[nodiscard]] bool CObjectAtspi::IsEnabled() const {
	return false;
}

[[nodiscard]] unsigned int CObjectAtspi::GetState() const {
	return 0;
}

[[nodiscard]] bool CObjectAtspi::HasState(IObject::EObjectState state) const {
	return false;
}

bool CObjectAtspi::HandleEvent(const CEvent& event) {
	return false;
}

[[nodiscard]] IObject* CObjectAtspi::GetParent() const {
	return nullptr;
}

[[nodiscard]] const std::vector<IObject*>& CObjectAtspi::GetChildren() const {
	return m_children;
}

[[nodiscard]] SRect CObjectAtspi::GetBounds() const {
	return SRect{};
}

[[nodiscard]] int CObjectAtspi::GetTabIndex() const {
	return -1;
}

[[nodiscard]] std::string CObjectAtspi::GetApplicationName() const {
	return "";
}

[[nodiscard]] std::string CObjectAtspi::GetName() const {
	return "";
}

[[nodiscard]] std::string CObjectAtspi::GetDescription() const {
	return "";
}

[[nodiscard]] std::string CObjectAtspi::GetText() const {
	return "";
}

[[nodiscard]] IObject* CObjectAtspi::GetSelectedItem() const {
	return nullptr;
}

[[nodiscard]] const std::vector<IObject*>& CObjectAtspi::GetItems() const {
	return m_children;
}

[[nodiscard]] double CObjectAtspi::GetMinValue() const {
	return 0.0;
}

[[nodiscard]] double CObjectAtspi::GetMaxValue() const {
	return 0.0;
}

[[nodiscard]] double CObjectAtspi::GetCurrentValue() const {
	return 0.0;
}
