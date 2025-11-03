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
