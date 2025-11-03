#include "Object.h"
#include <atspi/atspi.h>

[[nodiscard]] static constexpr inline IObject::EObjectType GetObjectTypeFromAtspiRole(const AtspiRole& role) {
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
		case ATSPI_ROLE_LIST_ITEM:
			return IObject::LIST_ITEM;
		case ATSPI_ROLE_MENU:
			return IObject::MENU;
		case ATSPI_ROLE_MENU_ITEM:
		case ATSPI_ROLE_CHECK_MENU_ITEM:
			return IObject::MENU_ITEM;
		case ATSPI_ROLE_PANEL:
			return IObject::PANEL;
		case ATSPI_ROLE_BUTTON:
			return IObject::BUTTON;
		case ATSPI_ROLE_RADIO_BUTTON:
			return IObject::RADIO_BUTTON;
		case ATSPI_ROLE_PROGRESS_BAR:
			return IObject::PROGRESS_BAR;
		case ATSPI_ROLE_WINDOW:
		case ATSPI_ROLE_DESKTOP_FRAME:
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

[[nodiscard]] static constexpr inline IObject::EObjectState GetObjectStateFromAtspiState(const AtspiStateType& state) {
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
		case ATSPI_STATE_MULTI_LINE:
			return IObject::MULTI_LINE;
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

		case ATSPI_STATE_EDITABLE:
			return IObject::EDITABLE;
		case ATSPI_STATE_EXPANDABLE:
			return IObject::EXPANDABLE;
		case ATSPI_STATE_FOCUSABLE:
			return IObject::FOCUSABLE;
		case ATSPI_STATE_SELECTABLE:
			return IObject::SELECTABLE;
		case ATSPI_STATE_MULTISELECTABLE:
			return IObject::MULTI_SELECTABLE;
		case ATSPI_STATE_RESIZABLE:
			return IObject::RESIZABLE;
		case ATSPI_STATE_CHECKABLE:
			return IObject::CHECKABLE;
		default:
			return IObject::NO;
	}
}

[[nodiscard]] static constexpr inline IObject::EObjectState GetObjectStateFromAtspiStates(const std::vector<AtspiStateType>& states) {
	unsigned int result = IObject::NO;
	for (const auto& state : states) {
		result |= GetObjectStateFromAtspiState(state);
	}
	return static_cast<IObject::EObjectState>(result);
}

[[nodiscard]] static inline IObject::EObjectState GetObjectStateFromAtspiStates(AtspiStateSet* states) {
	GArray* array = atspi_state_set_get_states(states);
	if (!array) [[unlikely]] {
		return IObject::NO;
	}

	std::vector<AtspiStateType> state_types;
	for (int i = 0; i < array->len; ++i) {
		gint64 state = g_array_index(array, gint64, i);
		state_types.push_back(static_cast<AtspiStateType>(state));
	}

	g_array_free(array, TRUE);
	g_object_unref(states);
	return GetObjectStateFromAtspiStates(state_types);
}

class CObjectAtspi final : public IObject {
	AtspiAccessible* m_accessible{nullptr};
	GArray* m_relations{nullptr};

	[[nodiscard]] std::vector<AtspiRelation> GetRelations();
public:
	explicit CObjectAtspi(AtspiAccessible* accessible) : m_accessible(accessible) {}
	~CObjectAtspi() override {
		if (m_relations) g_array_free(m_relations, TRUE);
	}

	[[nodiscard]] void* GetNativeHandle() override { return reinterpret_cast<void*>(m_accessible); }

	[[nodiscard]] inline bool IsValid() override { return m_accessible != nullptr; }

	[[nodiscard]] EObjectType GetType() override;
	[[nodiscard]] bool IsVisible() override;
	[[nodiscard]] bool IsEnabled() override;

	[[nodiscard]] EObjectState GetState() override;
	[[nodiscard]] bool HasState(EObjectState state) override;

	[[nodiscard]] std::weak_ptr<IObject> GetParent() override;
	[[nodiscard]] const std::vector<std::shared_ptr<IObject>>& GetChildren() override;

	[[nodiscard]] SRect GetBounds() override;

	[[nodiscard]] int GetTabIndex() override;

	[[nodiscard]] std::string GetApplicationName() override;

	[[nodiscard]] std::string GetName() override;
	[[nodiscard]] std::string GetDescription() override;
	[[nodiscard]] std::string GetText() override;

	[[nodiscard]] double GetMinValue() override;
	[[nodiscard]] double GetMaxValue() override;
	[[nodiscard]] double GetCurrentValue() override;

private:
	GError* m_lastError{nullptr};
};

