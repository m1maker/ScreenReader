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
		case BUTTON: return "button";
		case TEXT_FIELD: return "text field";
		case LABEL: return require_all ? "label" : "";
		case CHECKBOX: return "checkbox";
		case RADIO_BUTTON: return "radio button";
		case COMBO_BOX: return "combo box";
		case LIST_BOX: return "list box";
		case LIST_ITEM: return require_all ? "list item" : "";
		case MENU: return "menu";
		case MENU_ITEM: return require_all ? "menu item" : "";
		case SLIDER: return "slider";
		case PROGRESS_BAR: return "progress bar";
		case IMAGE: return "image";
		case PANEL: return "panel";
		case WINDOW: return "window";
		case DIALOG: return "dialog";
		default: return "unknown";
	}
}

[[nodiscard]] std::vector<std::string> IObject::GetStateNames(const IObject::EObjectState& states, bool require_all) {
	std::vector<std::string> state_names;
	if (states & VISIBLE && require_all) state_names.push_back("visible");
	if (states & ENABLED && require_all) state_names.push_back("enabled");
	if (states & FOCUSED && require_all) state_names.push_back("focused");
	if (states & SELECTED) state_names.push_back("selected");
	if (states & CHECKED) state_names.push_back("checked");
	if (states & EXPANDED) state_names.push_back("expanded");
	if (states & READONLY) state_names.push_back("read-only");
	if (states & MULTI_LINE) state_names.push_back("multi-line");
	if (states & SECURE) state_names.push_back("secure");
	if (states & REQUIRED) state_names.push_back("required");
	if (states & INVALID) state_names.push_back("invalid");
	if (states & HOVERED) state_names.push_back("hovered");
	if (states & PRESSED) state_names.push_back("pressed");
	if (states & DEFAULT) state_names.push_back("default");
	if (states & LOADING) state_names.push_back("loading");
	if (states & COLLAPSED) state_names.push_back("collapsed");

	if (states & EDITABLE && require_all) state_names.push_back("editable");
	if (states & EXPANDABLE && require_all) state_names.push_back("expandable");
	if (states & FOCUSABLE && require_all) state_names.push_back("focusable");
	if (states & SELECTABLE && require_all) state_names.push_back("selectable");
	if (states & MULTI_SELECTABLE) state_names.push_back("multi-selectable");
	if (states & RESIZABLE && require_all) state_names.push_back("resizable");
	if (states & CHECKABLE && require_all) state_names.push_back("checkable");

	return state_names;
}
