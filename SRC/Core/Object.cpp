// Some implementations of common object methods.
module;
#include <iomanip>
#include <sstream>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
module Core.Object;

/*
Just convert types and states to strings.

require_all parameter is used to determine whether we request names/states simply to announce focus changes, or whether
we log all states and names, or force the screen reader to request this.
*/
[[nodiscard]] auto GetObjectTypeName(EObjectType type, bool require_all) -> std::string_view {
	using enum EObjectType;
	switch (type) {
	case ALERT:
		return "alert";
	case ARTICLE:
		return "article";
	case AUDIO:
		return "audio";
	case AUTO_COMPLETE:
		return "auto complete";
	case BANNER:
		return "banner";
	case BLOCKQUOTE:
		return "blockquote";
	case BUTTON:
		return "button";
	case CANVAS:
		return "canvas";
	case CAPTION:
		return "caption";
	case CELL:
		return require_all ? "cell" : "";
	case CHECKBOX:
		return "checkbox";
	case CHART:
		return "chart";
	case COLOR_WELL:
		return "color well";
	case COLUMN_HEADER:
		return "column header";
	case COMBO_BOX:
		return "combo box";
	case COMMENT:
		return "comment";
	case DIALOG:
		return "dialog";
	case DOCUMENT:
		return "document";
	case FOOTER:
		return "footer";
	case FORM:
		return "form";
	case GRID:
		return "grid";
	case HEADER:
		return "header";
	case HEADING:
		return "heading";
	case IMAGE:
		return "image";
	case LABEL:
		return require_all ? "label" : "";
	case LINK:
		return "link";
	case LIST:
		return "list";
	case LIST_BOX:
		return "list box";
	case LIST_ITEM:
		return require_all ? "list item" : "";
	case MENU:
		return "menu";
	case MENU_BAR:
		return "menu bar";
	case MENU_ITEM:
		return require_all ? "menu item" : "";
	case PANEL:
		return "panel";
	case PROGRESS_BAR:
		return "progress bar";
	case RADIO_BUTTON:
		return "radio button";
	case ROW:
		return "row";
	case ROW_HEADER:
		return "row header";
	case SCROLL_BAR:
		return "scroll bar";
	case SECTION:
		return "section";
	case SLIDER:
		return "slider";
	case SPIN_BUTTON:
		return "spin button";
	case STATUS:
		return "status";
	case SWITCH:
		return "switch";
	case TAB:
		return "tab";
	case TAB_LIST:
		return "tab list";
	case TABLE:
		return "table";
	case TEXT_FIELD:
		return "text field";
	case TOGGLE_BUTTON:
		return "toggle button";
	case TOOLBAR:
		return "toolbar";
	case TOOLTIP:
		return "tooltip";
	case TREE:
		return "tree";
	case VIDEO:
		return "video";
	case WEB_VIEW:
		return "web view";
	case WINDOW:
		return "window";

	case UNKNOWN:
	default:
		return require_all ? "unknown" : "";
	}
}

/*
The object type query also applies to state names.
For example, there's no such state as EObjectState::UNCHECKED, but there is CHECKABLE.
We need to understand what kind of object this is to more accurately determine the states for announcements when we
don't request `require_all`.
*/
void GetObjectStateNames(std::pmr::string& out, EObjectType type, ObjectStates states, bool require_all) {
	// Capability / Infrastructure States (Usually only for logging/require_all).
	if (require_all) {
		if (states[std::to_underlying(EObjectState::VISIBLE)])
			out += " visible";
		if (states[std::to_underlying(EObjectState::ENABLED)])
			out += " enabled";
		if (states[std::to_underlying(EObjectState::FOCUSABLE)])
			out += " focusable";
		if (states[std::to_underlying(EObjectState::FOCUSED)])
			out += " focused";
		if (states[std::to_underlying(EObjectState::SELECTABLE)])
			out += " selectable";
		if (states[std::to_underlying(EObjectState::CHECKABLE)])
			out += " checkable";
		if (states[std::to_underlying(EObjectState::EDITABLE)])
			out += " editable";
		if (states[std::to_underlying(EObjectState::EXPANDABLE)])
			out += " expandable";
		if (states[std::to_underlying(EObjectState::RESIZABLE)])
			out += " resizable";
	}

	// Interactive / Crucial States (Always announced or contextually forced).
	if (states[std::to_underlying(EObjectState::BUSY)])
		out += " busy";
	if (states[std::to_underlying(EObjectState::LOADING)])
		out += " loading";

	// Selection Logic.
	if (states[std::to_underlying(EObjectState::SELECTED)]) {
		out += " selected";
	}

	// Toggle/Checked Logic (Normalization).
	// For Toggle Buttons, we prefer "pressed" over "checked".
	if (type == EObjectType::TOGGLE_BUTTON) {
		if (states[std::to_underlying(EObjectState::PRESSED)]) {
			out += " pressed";
		}
		else {
			out += " not pressed";
		}
	}

	// For Checkboxes (and generic checkables that aren't toggle buttons).
	else {
		if (states[std::to_underlying(EObjectState::CHECKED)]) {
			out += " checked";
		}
		else if (states[std::to_underlying(EObjectState::INDETERMINATE)]) {
			out += " partially checked";
		}
		else if (type == EObjectType::CHECKBOX ||
			(states[std::to_underlying(EObjectState::CHECKABLE)] && require_all)) {
			out += " not checked";
		}

		// Handle non-toggle-button 'pressed' state (e.g. normal button).
		if (states[std::to_underlying(EObjectState::PRESSED)])
			out += " pressed";
	}

	// Expansion Logic.
	if (states[std::to_underlying(EObjectState::EXPANDED)]) {
		out += " expanded";
	}
	else if (states[std::to_underlying(EObjectState::COLLAPSED)]) {
		out += " collapsed";
	}

	// Text / Input specific normalization.
	if (states[std::to_underlying(EObjectState::READONLY)])
		out += " read-only";
	if (states[std::to_underlying(EObjectState::SECURE)])
		out += " secure";
	if (states[std::to_underlying(EObjectState::INVALID)])
		out += " invalid";
	if (states[std::to_underlying(EObjectState::REQUIRED)]) {
		// Only announce "required" for input types unless logging all.
		if (IsObjectInput(type) || require_all) {
			out += " required";
		}
	}

	if (states[std::to_underlying(EObjectState::MULTI_LINE)])
		out += " multi-line";
	if (states[std::to_underlying(EObjectState::MULTI_SELECTABLE)])
		out += " multi-selectable";

	// Global attributes.
	if (states[std::to_underlying(EObjectState::HOVERED)] && require_all)
		out += " hovered";
	if (states[std::to_underlying(EObjectState::DEFAULT)])
		out += " default";
	if (states[std::to_underlying(EObjectState::MODAL)])
		out += " modal";
	if (states[std::to_underlying(EObjectState::VISITED)])
		out += " visited";
}
