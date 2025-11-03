#include "Object.h"

[[nodiscard]] std::shared_ptr<IObject> FindFocusedObject(std::shared_ptr<IObject> start_from, bool force) {
	if (!start_from) [[unlikely]] return std::shared_ptr<IObject>();
	if (!force && start_from->GetState() & IObject::FOCUSED) return start_from;

	auto children = start_from->GetChildren();
	for (auto child : children) {
		if (child->GetState() & IObject::FOCUSED) return child;
		return FindFocusedObject(child, true);
	}

	return start_from;
}

// I need to make translations in the future, so don't make it constexpr or inline
[[nodiscard]] std::string IObject::GetTypeName(const IObject::EObjectType& type, bool require_all) {
	switch (type) {
		case BUTTON: return "Button";
		case TEXT_FIELD: return "Text field";
		case LABEL: return require_all ? "Label" : "";
		case CHECKBOX: return "Checkbox";
		case RADIO_BUTTON: return "Radio button";
		case COMBO_BOX: return "Combo box";
		case LIST_BOX: return "List box";
		case LIST_ITEM: return require_all ? "List item" : "";
		case MENU: return "Menu";
		case MENU_ITEM: return require_all ? "Menu item" : "";
		case SLIDER: return "Slider";
		case PROGRESS_BAR: return "Progress bar";
		case IMAGE: return "Image";
		case PANEL: return "Panel";
		case WINDOW: return "Window";
		default: return "Unknown";
	}
}

[[nodiscard]] std::vector<std::string> IObject::GetStateNames(const IObject::EObjectState& states, bool require_all) {
	std::vector<std::string> state_names;
	if (states & VISIBLE && require_all) state_names.push_back("Visible");
	if (states & ENABLED && require_all) state_names.push_back("Enabled");
	if (states & FOCUSED && require_all) state_names.push_back("Focused");
	if (states & SELECTED) state_names.push_back("Selected");
	if (states & CHECKED) state_names.push_back("Checked");
	if (states & EXPANDED) state_names.push_back("Expanded");
	if (states & READONLY) state_names.push_back("Read-only");
	if (states & REQUIRED) state_names.push_back("Required");
	if (states & INVALID) state_names.push_back("Invalid");
	if (states & HOVERED) state_names.push_back("Hovered");
	if (states & PRESSED) state_names.push_back("Pressed");
	if (states & DEFAULT) state_names.push_back("Default");
	if (states & LOADING) state_names.push_back("Loading");
	if (states & COLLAPSED) state_names.push_back("Collapsed");

	return state_names;
}
