// Some implementations of common object methods.
#include "Object.h"
#include <sstream>
#include <iomanip>
#include <utility>
#include <type_traits>

void IObject::UpdateCacheByEvent(const CEvent::EEventType& event) {
	switch (event) {
		case CEvent::VALUE_CHANGED:
			Cache(m_minValue, std::nullopt);
			Cache(m_maxValue, std::nullopt);
			Cache(m_currentValue, std::nullopt);
			break;
		//case CEvent::SELECTION_CHANGED:
		case CEvent::STATE_CHANGED:
		case CEvent::VISIBILITY_CHANGED:
		case CEvent::ENABLED_CHANGED:
			Cache(m_states, std::nullopt);
			break;
		//case CEvent::TEXT_CHANGED:
			//Cache(m_text, std::nullopt);
			//break;
		case CEvent::CURSOR_MOVED:
			Cache(m_cursor, std::nullopt);
			break;
		case CEvent::CHILD_ADDED:
		case CEvent::CHILD_REMOVED:
			Cache(m_children, std::nullopt);
			break;
		case CEvent::PARENT_UPDATED:
			Cache(m_parent, std::nullopt);
			break;
		default: break;
	}
}

// I need to make translations in the future, so don't make it constexpr or inline
/*
Just convert types and states to strings.

require_all parameter is used to determine whether we request names/states simply to announce focus changes, or whether we log all states and names, or force the screen reader to request this.
*/
[[nodiscard]] auto IObject::GetTypeName(const IObject::EObjectType& type, bool require_all) -> std::string {
	switch (type) {
		case ALERT:           return "alert";
		case ARTICLE:         return "article";
		case AUDIO:           return "audio";
		case AUTO_COMPLETE:   return "auto complete";
		case BANNER:          return "banner";
		case BLOCKQUOTE:      return "blockquote";
		case BUTTON:          return "button";
		case CANVAS:          return "canvas";
		case CAPTION:         return "caption";
		case CELL: return require_all ? "cell" : "";
		case CHECKBOX:        return "checkbox";
		case CHART:           return "chart";
		case COLOR_WELL:      return "color well";
		case COLUMN_HEADER:   return "column header";
		case COMBO_BOX:       return "combo box";
		case COMMENT:         return "comment";
		case DIALOG:          return "dialog";
		case DOCUMENT:        return "document";
		case FOOTER:          return "footer";
		case FORM:            return "form";
		case GRID:            return "grid";
		case HEADER:          return "header";
		case HEADING:         return "heading";
		case IMAGE:           return "image";
		case LABEL:           return require_all ? "label" : "";
		case LINK:            return "link";
		case LIST:            return "list";
		case LIST_BOX:        return "list box";
		case LIST_ITEM:       return require_all ? "list item" : "";
		case MENU:            return "menu";
		case MENU_BAR:        return "menu bar";
		case MENU_ITEM:       return require_all ? "menu item" : "";
		case PANEL:           return "panel";
		case PROGRESS_BAR:    return "progress bar";
		case RADIO_BUTTON:    return "radio button";
		case ROW:             return "row";
		case ROW_HEADER:      return "row header";
		case SCROLL_BAR:      return "scroll bar";
		case SECTION:         return "section";
		case SLIDER:          return "slider";
		case SPIN_BUTTON:     return "spin button";
		case STATUS:          return "status";
		case SWITCH:          return "switch";
		case TAB:             return "tab";
		case TAB_LIST:        return "tab list";
		case TABLE:           return "table";
		case TEXT_FIELD:      return "text field";
		case TOGGLE_BUTTON:   return "toggle button";
		case TOOLBAR:         return "toolbar";
		case TOOLTIP:         return "tooltip";
		case TREE:            return "tree";
		case VIDEO:           return "video";
		case WEB_VIEW:        return "web view";
		case WINDOW:          return "window";

		case UNKNOWN:
		default:              return require_all ? "unknown" : "";
	}
}

/*
The object type query also applies to state names.
For example, there's no such state as EObjectState::UNCHECKED, but there is CHECKABLE.
We need to understand what kind of object this is to more accurately determine the states for announcements when we don't request `require_all`.
*/
[[nodiscard]] auto IObject::GetStateNames(const IObject::EObjectType& type, const unsigned long long& states, bool require_all) -> std::vector<std::string> {
	std::vector<std::string> state_names;

	// Capability / Infrastructure States (Usually only for logging/require_all).
	if (require_all) {
		if (states & VISIBLE)          state_names.emplace_back("visible");
		if (states & ENABLED)          state_names.emplace_back("enabled");
		if (states & FOCUSABLE)        state_names.emplace_back("focusable");
		if (states & FOCUSED)              state_names.emplace_back("focused");
		if (states & SELECTABLE)       state_names.emplace_back("selectable");
		if (states & CHECKABLE)        state_names.emplace_back("checkable");
		if (states & EDITABLE)         state_names.emplace_back("editable");
		if (states & EXPANDABLE)       state_names.emplace_back("expandable");
		if (states & RESIZABLE)        state_names.emplace_back("resizable");
	}

	// Interactive / Crucial States (Always announced or contextually forced).
	if (states & BUSY)                 state_names.emplace_back("busy");
	if (states & LOADING)              state_names.emplace_back("loading");

	// Selection Logic.
	if (states & SELECTED) {
		state_names.emplace_back("selected");
	}

	// Toggle/Checked Logic (Normalization).
	// For Toggle Buttons, we prefer "pressed" over "checked".
	if (type == TOGGLE_BUTTON) {
		if (states & PRESSED) {
			state_names.emplace_back("pressed");
		}
		else {
			state_names.emplace_back("not pressed");
		}
	} 

	// For Checkboxes (and generic checkables that aren't toggle buttons).
	else {
		if (states & CHECKED) {
			state_names.emplace_back("checked");
		}
		else if (states & INDETERMINATE) {
			state_names.emplace_back("partially checked");
		}
		else if (type == CHECKBOX || (states & CHECKABLE && require_all)) {
			state_names.emplace_back("not checked");
		}

		// Handle non-toggle-button 'pressed' state (e.g. normal button).
		if (states & PRESSED) state_names.emplace_back("pressed");
	}

	// Expansion Logic.
	if (states & EXPANDED) {
		state_names.emplace_back("expanded");
	}
	else if (states & COLLAPSED) {
		state_names.emplace_back("collapsed");
	}

	// Text / Input specific normalization.
	if (states & READONLY)             state_names.emplace_back("read-only");
	if (states & SECURE)               state_names.emplace_back("secure");
	if (states & INVALID)              state_names.emplace_back("invalid");
	if (states & REQUIRED) {
		// Only announce "required" for input types unless logging all.
		if (type == TEXT_FIELD || type == COMBO_BOX || type == LIST_BOX || require_all) {
			state_names.emplace_back("required");
		}
	}

	if (states & MULTI_LINE)           state_names.emplace_back("multi-line");
	if (states & MULTI_SELECTABLE)     state_names.emplace_back("multi-selectable");

	// Global attributes.
	if (states & HOVERED && require_all) state_names.emplace_back("hovered");
	if (states & DEFAULT) state_names.emplace_back("default");
	if (states & MODAL)                  state_names.emplace_back("modal");
	if (states & VISITED)                state_names.emplace_back("visited");

	return state_names;
}

/*
This function is used to represent an IObject as an std::string.
Currently, it is only used in the logger.
*/
[[nodiscard]] auto DumpObjectToString(const std::shared_ptr<IObject>& obj, int indent, bool recursive, int max_depth, int current_depth) -> std::string {
	if (!obj) {
		return std::string(indent, ' ') + "[NULL OBJECT]\n";
	}

	if (current_depth > max_depth) {
		return std::string(indent, ' ') + "[MAX DEPTH REACHED]\n";
	}

	std::ostringstream oss;
	std::string indent_string(indent, ' ');

	oss << indent_string << "IObject@" << std::hex << std::setw(16) << std::setfill('0') 
		<< reinterpret_cast<uintptr_t>(obj.get()) << std::dec << " {\n";

	auto type = obj->GetType();
	if (!type) {
		auto last_error = type.error();
		oss << indent_string << "  Error: " <<
				IObject::ErrorToString(last_error) << "\n";
		return oss.str();
	}	

	oss << indent_string << "  Type: " << IObject::GetTypeName(type.value_or(IObject::UNKNOWN), true) 
		<< " (" << static_cast<int>(type.value_or(IObject::UNKNOWN)) << ")\n";

	unsigned long long states = obj->GetState().value_or(IObject::NO);
	auto state_names = IObject::GetStateNames(type.value_or(IObject::UNKNOWN), states, true);
	oss << indent_string << "  States: 0x" << std::hex << std::setw(16) << std::setfill('0') 
		<< states << std::dec << " [";
	for (size_t i = 0; std::cmp_less(i, state_names.size()); ++i) {
		if (i > 0) oss << ", ";
		oss << state_names[i];
	}
	oss << "]\n";

	oss << indent_string << "  Name: \"" << obj->GetName().value_or("") << "\"\n";
	oss << indent_string << "  Description: \"" << obj->GetDescription().value_or("") << "\"\n";
	//oss << indent_string << "  Text: \"" << obj->GetText() << "\"\n";

	oss << indent_string << "  App: \"" << obj->GetApplicationName().value_or("") << "\"\n";

	if (auto bounds = obj->GetBounds()) {
		oss << indent_string << "  Bounds: (" << bounds->x << ", " << bounds->y << ") "
				<< bounds->width << "x" << bounds->height << "\n";
	}

	double min_value = obj->GetMinValue().value_or(0);
	double max_value = obj->GetMaxValue().value_or(0);
	double current_value = obj->GetCurrentValue().value_or(0);
	if (min_value != 0.0 || max_value != 0.0 || current_value != 0.0) {
		oss << indent_string << "  Value: " << current_value << " [" << min_value << " - " << max_value << "]\n";
	}

	oss << indent_string << "  Index: " << obj->GetIndex().value_or(0) << "\n";

	int cursor = obj->GetCursor().value_or(0);
	if (cursor >= 0) {
		oss << indent_string << "  Cursor: " << cursor << "\n";
	}

	oss << "\n";

	void* native_handle = obj->GetNativeHandle().value_or(nullptr);
	if (native_handle) {
		oss << indent_string << "  NativeHandle: 0x" << std::hex 
			<< reinterpret_cast<uintptr_t>(native_handle) << std::dec << "\n";
	}

	if (auto children = obj->GetChildren()) {
		oss << indent_string << "  Children: " << children->size() << "\n";

		oss << indent_string << "}";

		if (recursive && !children->empty()) {
			oss << "\n" << indent_string << "Children details:\n";
			for (size_t i = 0; std::cmp_less(i, children->size()); ++i) {
				oss << indent_string << "[" << i << "] ";
				std::string child_dump = DumpObjectToString(
					children->operator[](i), 
					indent + 4, 
					recursive, 
					max_depth, 
					current_depth + 1
				);

				size_t first_newline = child_dump.find('\n');
				if (first_newline != std::string::npos) {
					oss << child_dump.substr(indent + 4, first_newline - (indent + 4));
					oss << child_dump.substr(first_newline);
				}
				else {
					oss << child_dump;
				}
				if (i < children->size() - 1) {
					oss << "\n";
				}
			}
		}
	}

	return oss.str();
}

