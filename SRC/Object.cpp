// Some implementations of common object methods.
#include "Object.h"

/*
Attempt to find the focused object.
The force parameter is used to continue searching for the object, regardless of the first one found.
*/
[[nodiscard]] auto FindFocusedObject(std::shared_ptr<IObject> start_from, bool force) -> std::shared_ptr<IObject> {
	if (!start_from) [[unlikely]] return nullptr;

	if (!force && (start_from->GetState() & IObject::FOCUSED)) {
		return start_from;
	}

	auto children = start_from->GetChildren();
	for (auto& child : children) {
		if (child->GetState() & IObject::FOCUSED) {
			return child;
		}
	}

	for (auto& child : children) {
		if (auto result = FindFocusedObject(child, true)) {
			return result;
		}
	}

	return (force) ? nullptr : start_from;
}

// I need to make translations in the future, so don't make it constexpr or inline
/*
Just convert types and states to strings.

require_all parameter is used to determine whether we request names/states simply to announce focus changes, or whether we log all states and names, or force the screen reader to request this.
*/
[[nodiscard]] auto IObject::GetTypeName(const IObject::EObjectType& type, bool require_all) -> std::string {
	switch (type) {
		case BUTTON: return "button";
		case TOGGLE_BUTTON: return "toggle button";
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

/*
The object type query also applies to state names.
For example, there's no such state as EObjectState::UNCHECKED, but there is CHECKABLE.
We need to understand what kind of object this is to more accurately determine the states for announcements when we don't request `require_all`.
*/
[[nodiscard]] auto IObject::GetStateNames(const IObject::EObjectType& type, const unsigned long long& states, bool require_all) -> std::vector<std::string> {
	std::vector<std::string> state_names;
	if (states & VISIBLE && require_all) state_names.emplace_back("visible");
	if (states & ENABLED && require_all) state_names.emplace_back("enabled");
	if (states & FOCUSED && require_all) state_names.emplace_back("focused");
	if (states & SELECTED) {
		state_names.emplace_back("selected");
	}
	/*
	A checkbox cannot be without checked, unchecked, or partially checked state.
	However, for example, AT-SPI does not have an unchecked or partially checked state.
	Let's try to standardize this.

	With AT-SPI, I also found a toggle button with a checked state.
	No! It should be either pressed or not pressed.
	*/
	if (states & CHECKED && type != TOGGLE_BUTTON) {
		state_names.emplace_back("checked");
	}
	else if (type == CHECKBOX) { // Ignore CHECKABLE state. So it is a checkbox
		state_names.emplace_back("not checked");
	}
	if (states & EXPANDED) state_names.emplace_back("expanded");
	if (states & READONLY) state_names.emplace_back("read-only");
	if (states & MULTI_LINE) state_names.emplace_back("multi-line");
	if (states & SECURE) state_names.emplace_back("secure");
	if (states & REQUIRED && type == TEXT_FIELD) state_names.emplace_back("required");
	if (states & INVALID) state_names.emplace_back("invalid");
	if (states & HOVERED) state_names.emplace_back("hovered");
	if (states & PRESSED) {
		state_names.emplace_back("pressed");
	}
	else if (type == TOGGLE_BUTTON) { // Same as checkboxes
		state_names.emplace_back("not pressed");
	}
	if (states & DEFAULT) state_names.emplace_back("default");
	if (states & LOADING) state_names.emplace_back("loading");
	if (states & COLLAPSED) state_names.emplace_back("collapsed");

	if (states & EDITABLE && require_all) state_names.emplace_back("editable");
	if (states & EXPANDABLE && require_all) state_names.emplace_back("expandable");
	if (states & FOCUSABLE && require_all) state_names.emplace_back("focusable");
	if (states & SELECTABLE && require_all) state_names.emplace_back("selectable");
	if (states & MULTI_SELECTABLE) state_names.emplace_back("multi-selectable");
	if (states & RESIZABLE && require_all) state_names.emplace_back("resizable");
	if (states & CHECKABLE && require_all) state_names.emplace_back("checkable");

	return state_names;
}
