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
void GetObjectStateNames(std::pmr::string& out, EObjectType type, unsigned long long state, bool require_all) {
	using enum EObjectType;
	using enum EObjectState;

	auto states = static_cast<EObjectState>(state);
	// Capability / Infrastructure States (Usually only for logging/require_all).
	if (require_all) {
		if (!!(states & VISIBLE))
			out += " visible";
		if (!!(states & ENABLED))
			out += " enabled";
		if (!!(states & FOCUSABLE))
			out += " focusable";
		if (!!(states & FOCUSED))
			out += " focused";
		if (!!(states & SELECTABLE))
			out += " selectable";
		if (!!(states & CHECKABLE))
			out += " checkable";
		if (!!(states & EDITABLE))
			out += " editable";
		if (!!(states & EXPANDABLE))
			out += " expandable";
		if (!!(states & RESIZABLE))
			out += " resizable";
	}

	// Interactive / Crucial States (Always announced or contextually forced).
	if (!!(states & BUSY))
		out += " busy";
	if (!!(states & LOADING))
		out += " loading";

	// Selection Logic.
	if (!!(states & SELECTED)) {
		out += " selected";
	}

	// Toggle/Checked Logic (Normalization).
	// For Toggle Buttons, we prefer "pressed" over "checked".
	if (type == TOGGLE_BUTTON) {
		if (!!(states & PRESSED)) {
			out += " pressed";
		}
		else {
			out += " not pressed";
		}
	}

	// For Checkboxes (and generic checkables that aren't toggle buttons).
	else {
		if (!!(states & CHECKED)) {
			out += " checked";
		}
		else if (!!(states & INDETERMINATE)) {
			out += " partially checked";
		}
		else if (type == CHECKBOX || (!!(states & CHECKABLE) && require_all)) {
			out += " not checked";
		}

		// Handle non-toggle-button 'pressed' state (e.g. normal button).
		if (!!(states & PRESSED))
			out += " pressed";
	}

	// Expansion Logic.
	if (!!(states & EXPANDED)) {
		out += " expanded";
	}
	else if (!!(states & COLLAPSED)) {
		out += " collapsed";
	}

	// Text / Input specific normalization.
	if (!!(states & READONLY))
		out += " read-only";
	if (!!(states & SECURE))
		out += " secure";
	if (!!(states & INVALID))
		out += " invalid";
	if (!!(states & REQUIRED)) {
		// Only announce "required" for input types unless logging all.
		if (IsObjectInput(type) || require_all) {
			out += " required";
		}
	}

	if (!!(states & MULTI_LINE))
		out += " multi-line";
	if (!!(states & MULTI_SELECTABLE))
		out += " multi-selectable";

	// Global attributes.
	if (!!(states & HOVERED) && require_all)
		out += " hovered";
	if (!!(states & DEFAULT))
		out += " default";
	if (!!(states & MODAL))
		out += " modal";
	if (!!(states & VISITED))
		out += " visited";
}
