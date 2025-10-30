export module ObjectAtspi;
#include <atspi.h>
#include "Object.h"
#include <vector>

[[nodiscard]] static constexpr IObject::EObjectType GetObjectTypeFromAtspiRole(const AtspiRole& role) {
	switch (role) {
		case ATSPI_ROLE_INVALID:
			return IObject::UNKNOWN;
		case ATSPI_ROLE_CHECK_BOX:
			return IObject::CHECKBOX;
		case ATSPI_ROLE_COMBO_BOX:
			return IObject::COMBO_BOX;
		case ATSPI_ROLE_LABEL:
			return IObject::LABEL;
		case ATSPI_ROLE_LIST:
			return IObject::LIST_BOX;
		case ATSPI_ROLE_PANEL:
			return IObject::PANEL;
		case ATSPI_ROLE_BUTTON:
		case ATSPI_ROLE_PUSH_BUTTON:
			return IObject::BUTTON;
		case ATSPI_ROLE_RADIO_BUTTON:
			return IObject::RADIO_BUTTON;
		case ATSPI_ROLE_PROGRESS_BAR:
			return IObject::PROGRESS_BAR;
		case ATSPI_ROLE_WINDOW:
			return IObject::WINDOW;
		case ATSPI_ROLE_IMAGE:
			return IObject::IMAGE;
		case ATSPI_ROLE_SLIDER:
			return IObject::SLIDER;
		case ATSPI_ROLE_TEXT:
		case ATSPI_ROLE_ENTRY:
		case ATSPI_ROLE_PASSWORD_TEXT:
			return IObject::TEXT_FIELD;
		default:
			return IObject::UNKNOWN;
	}
}

[[nodiscard]] static constexpr unsigned int GetObjectStateFromAtspiState(const AtspiStateType& state) {
	switch (state) {
		case ATSPI_STATE_VISIBLE:
			return IObject::VISIBLE;
		case ATSPI_STATE_ENABLED:
			return IObject::ENABLED;
		case ATSPI_STATE_FOCUSED:
			return IObject::FOCUSED;
		case ATSPI_STATE_SELECTED:
			return IObject::SELECTED;
		case ATSPI_STATE_CHECKED:
			return IObject::CHECKED;
		case ATSPI_STATE_EXPANDED:
			return IObject::EXPANDED;
		case ATSPI_STATE_READ_ONLY:
			return IObject::READONLY;
		case ATSPI_STATE_REQUIRED:
			return IObject::REQUIRED;
		case ATSPI_STATE_INVALID_ENTRY:
			return IObject::INVALID;
		case ATSPI_STATE_ARMED:
			case ATSPI_STATE_PRESSED:
			return IObject::PRESSED;
		case ATSPI_STATE_BUSY:
			return IObject::LOADING;
		case ATSPI_STATE_COLLAPSED:
			return IObject::COLLAPSED;
		case ATSPI_STATE_IS_DEFAULT:
			return IObject::DEFAULT;
		default:
			return 0;
	}
}

[[nodiscard]] static constexpr unsigned int GetObjectStateFromAtspiStates(const std::vector<AtspiStateType>& states) {
	unsigned int result = 0;
	for (const auto& state : states) {
		result |= GetObjectStateFromAtspiState(state);
	}
	return result;
}

export class CObjectAtspi final : public IObject {
	ATSPIAccessible* m_accessible{nullptr};
public:

};

