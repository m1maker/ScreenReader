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

/*
Attempt to find the focused object.
The force parameter is used to continue searching for the object, regardless of the first one found.
*/
[[nodiscard]] auto FindFocusedObject(std::shared_ptr<IObject> start_from, bool force) -> std::shared_ptr<IObject> {
	if (!start_from) [[unlikely]] return nullptr;

	if (!force && (start_from->GetState().value() & IObject::FOCUSED)) {
		return start_from;
	}

	auto children = start_from->GetChildren().value();
	for (auto& child : children) {
		if (child->GetState().value() & IObject::FOCUSED) {
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
		default: return require_all ? "unknown" : "";
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

	auto type = obj->GetType().value();
	oss << indent_string << "  Type: " << IObject::GetTypeName(type, true) 
		<< " (" << static_cast<int>(type) << ")\n";

	oss << indent_string << "  Valid: " << (obj->IsValid() ? "true" : "false") << "\n";
	oss << indent_string << "  Visible: " << (obj->IsVisible() ? "true" : "false") << "\n";
	oss << indent_string << "  Enabled: " << (obj->IsEnabled() ? "true" : "false") << "\n";

	unsigned long long states = obj->GetState().value();
	auto state_names = IObject::GetStateNames(type, states, true);
	oss << indent_string << "  States: 0x" << std::hex << std::setw(16) << std::setfill('0') 
		<< states << std::dec << " [";
	for (size_t i = 0; std::cmp_less(i, state_names.size()); ++i) {
		if (i > 0) oss << ", ";
		oss << state_names[i];
	}
	oss << "]\n";

	oss << indent_string << "  Name: \"" << obj->GetName().value() << "\"\n";
	oss << indent_string << "  Description: \"" << obj->GetDescription().value() << "\"\n";
	//oss << indent_string << "  Text: \"" << obj->GetText() << "\"\n";

	oss << indent_string << "  App: \"" << obj->GetApplicationName().value() << "\"\n";

	auto bounds = obj->GetBounds().value();
	oss << indent_string << "  Bounds: (" << bounds.x << ", " << bounds.y << ") "
		<< bounds.width << "x" << bounds.height << "\n";

	double min_value = obj->GetMinValue().value();
	double max_value = obj->GetMaxValue().value();
	double current_value = obj->GetCurrentValue().value();
	if (min_value != 0.0 || max_value != 0.0 || current_value != 0.0) {
		oss << indent_string << "  Value: " << current_value << " [" << min_value << " - " << max_value << "]\n";
	}

	oss << indent_string << "  Index: " << obj->GetIndex().value() << "\n";

	int cursor = obj->GetCursor().value();
	if (cursor >= 0) {
		oss << indent_string << "  Cursor: " << cursor << "\n";
	}

	auto parent = obj->GetParent()->lock();
	oss << indent_string << "  Parent: ";
	if (parent) {
		oss << parent->GetTypeName(parent->GetType().value(), true) << "@"
			<< std::hex << std::setw(16) << std::setfill('0') 
			<< reinterpret_cast<uintptr_t>(parent.get()) << std::dec;
	}
	else {
		oss << "[none]";
	}
	oss << "\n";

	const auto& children = obj->GetChildren().value();
	oss << indent_string << "  Children: " << children.size() << "\n";

	void* native_handle = obj->GetNativeHandle().value();
	if (native_handle) {
		oss << indent_string << "  NativeHandle: 0x" << std::hex 
			<< reinterpret_cast<uintptr_t>(native_handle) << std::dec << "\n";
	}

	oss << indent_string << "}";

	if (recursive && !children.empty()) {
		oss << "\n" << indent_string << "Children details:\n";
		for (size_t i = 0; std::cmp_less(i, children.size()); ++i) {
			oss << indent_string << "[" << i << "] ";
			std::string child_dump = DumpObjectToString(
				children[i], 
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
			if (i < children.size() - 1) {
				oss << "\n";
			}
		}
	}

	return oss.str();
}

