#include "Object.h"

// I need to make translations in the future, so don't make it constexpr or inline
[[nodiscard]] std::string IObject::GetTypeName(const IObject::EObjectType& type) {
	switch (type) {
		case BUTTON: return "Button";
		case TEXT_FIELD: return "Text field";
		case LABEL: return "Label";
		case CHECKBOX: return "Checkbox";
		case RADIO_BUTTON: return "Radio button";
		case COMBO_BOX: return "Combo box";
		case LIST_BOX: return "List box";
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
	if (require_all) {
		if (states & VISIBLE) state_names.push_back("Visible");
		if (states & ENABLED) state_names.push_back("Enabled");
		if (states & FOCUSED) state_names.push_back("Focused");
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
	}

	return state_names;
}
