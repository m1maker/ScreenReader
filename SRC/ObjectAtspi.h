// AT-SPI object implementation and some inlines.
#include "Object.h"
#include <atspi/atspi.h>

#include <utility>

[[nodiscard]] static constexpr inline auto GetObjectTypeFromAtspiRole(const AtspiRole& role) -> IObject::EObjectType {
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
		case ATSPI_ROLE_TOGGLE_BUTTON:
			return IObject::TOGGLE_BUTTON;
		case ATSPI_ROLE_PROGRESS_BAR:
			return IObject::PROGRESS_BAR;
		case ATSPI_ROLE_WINDOW:
		case ATSPI_ROLE_DESKTOP_FRAME:
		case ATSPI_ROLE_APPLICATION:
			return IObject::WINDOW;
		case ATSPI_ROLE_DIALOG:
			return IObject::DIALOG;
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

[[nodiscard]] static constexpr inline auto GetObjectStateFromAtspiState(const AtspiStateType& state) -> IObject::EObjectState {
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

[[nodiscard]] static constexpr inline auto GetObjectStateFromAtspiStates(const std::vector<AtspiStateType>& states) -> unsigned long long {
	unsigned long long result = IObject::NO;
	for (const auto& state : states) {
		result |= GetObjectStateFromAtspiState(state);
	}
	return result;
}

[[nodiscard]] static inline auto GetObjectStateFromAtspiStates(AtspiStateSet* states) -> unsigned long long {
	GArray* array = atspi_state_set_get_states(states);
	if (!array) [[unlikely]] {
		return IObject::NO;
	}

	std::vector<AtspiStateType> state_types;
	for (int i = 0; std::cmp_less(i , array->len); ++i) {
		gint64 state = g_array_index(array, gint64, i);
		state_types.push_back(static_cast<AtspiStateType>(state));
	}

	g_array_free(array, TRUE);
	return GetObjectStateFromAtspiStates(state_types);
}

/*
Wrapper class for GLib strings so that they are automatically freed.
*/
class CGlibString final {
private:
	gchar* m_pointer{nullptr};
public:

	constexpr explicit CGlibString(gchar* pointer) : m_pointer(pointer) {}

	CGlibString(const CGlibString&) = delete;
	auto operator=(const CGlibString&) -> CGlibString& = delete;

	CGlibString(CGlibString&& other)  noexcept : m_pointer(other.m_pointer) {
		other.m_pointer = nullptr;
	}

	~CGlibString() {
		if (m_pointer) {
			g_free(m_pointer);
		}
	}

	constexpr void reset(gchar* new_pointer = nullptr) {
		if (m_pointer) {
			g_free(m_pointer);
		}
		m_pointer = new_pointer;
	}

	auto operator=(CGlibString&& other)  noexcept -> CGlibString& {
		if (this != &other) {
			reset(other.m_pointer);
			other.m_pointer = nullptr;
		}
		return *this;
	}

	operator std::string() const {
		return m_pointer ? std::string(m_pointer) : std::string();
	}

	[[nodiscard]] inline constexpr auto c_str() const -> const char* { return m_pointer ? m_pointer : ""; }
	[[nodiscard]] constexpr inline auto empty() const -> bool { return !m_pointer || !*m_pointer; }
};

class CObjectAtspi final : public IObject {
	mutable AtspiAccessible* m_accessible{nullptr};

	mutable AtspiAction* m_actionInterface{nullptr};
	mutable AtspiCollection* m_collectionInterface{nullptr};
	mutable AtspiComponent* m_componentInterface{nullptr};
	mutable AtspiDocument* m_documentInterface{nullptr};
	mutable AtspiEditableText* m_editableTextInterface{nullptr};
	mutable AtspiHypertext* m_hypertextInterface{nullptr};
	mutable AtspiImage* m_imageInterface{nullptr};
	mutable AtspiSelection* m_selectionInterface{nullptr};
	mutable AtspiTable* m_tableInterface{nullptr};
	mutable AtspiText* m_textInterface{nullptr};
	mutable AtspiValue* m_valueInterface{nullptr};

	mutable GArray* m_relations{nullptr};

	inline void ResetLastError() const noexcept {
		if (m_lastError) {
			g_error_free(m_lastError);
			m_lastError = nullptr;
		}
	}

	[[nodiscard]] auto GetRelations() const noexcept -> std::vector<AtspiRelation>;
public:
	explicit CObjectAtspi(AtspiAccessible* accessible) : m_accessible(accessible) {}
	~CObjectAtspi() override {
		if (m_relations) g_array_free(m_relations, TRUE);
		if (m_accessible) g_object_unref(m_accessible);
		if (m_actionInterface) g_object_unref(m_actionInterface);
		if (m_collectionInterface) g_object_unref(m_collectionInterface);
		if (m_componentInterface) g_object_unref(m_componentInterface);
		if (m_documentInterface) g_object_unref(m_documentInterface);
		if (m_editableTextInterface) g_object_unref(m_editableTextInterface);
		if (m_hypertextInterface) g_object_unref(m_hypertextInterface);
		if (m_imageInterface) g_object_unref(m_imageInterface);
		if (m_selectionInterface) g_object_unref(m_selectionInterface);
		if (m_tableInterface) g_object_unref(m_tableInterface);
		if (m_textInterface) g_object_unref(m_textInterface);
		if (m_valueInterface) g_object_unref(m_valueInterface);

		ResetLastError();
	}

	[[nodiscard]] auto GetNativeHandle() const noexcept -> void* override { return reinterpret_cast<void*>(m_accessible); }

	[[nodiscard]] inline auto IsValid() const noexcept -> bool override { return m_accessible != nullptr; }

	[[nodiscard]] auto GetType() const noexcept -> EObjectType override;
	[[nodiscard]] auto IsVisible() const noexcept -> bool override;
	[[nodiscard]] auto IsEnabled() const noexcept -> bool override;

	[[nodiscard]] auto GetState() const noexcept -> unsigned long long override;
	[[nodiscard]] auto HasState(EObjectState state) const noexcept -> bool override;

	[[nodiscard]] auto GetParent() const noexcept -> std::weak_ptr<IObject> override;
	[[nodiscard]] auto GetChildren() const noexcept -> const std::vector<std::shared_ptr<IObject>>& override;

	[[nodiscard]] auto GetBounds() const noexcept -> SRect override;

	[[nodiscard]] auto GetTabIndex() const noexcept -> int override;

	[[nodiscard]] auto GetApplicationName() const noexcept -> std::string override;

	[[nodiscard]] auto GetName() const noexcept -> std::string override;
	[[nodiscard]] auto GetDescription() const noexcept -> std::string override;

	[[nodiscard]] auto GetCursor() const noexcept -> int override;
	[[nodiscard]] auto GetText(bool at_cursor = false) const noexcept -> std::string override;

	[[nodiscard]] auto GetMinValue() const noexcept -> double override;
	[[nodiscard]] auto GetMaxValue() const noexcept -> double override;
	[[nodiscard]] auto GetCurrentValue() const noexcept -> double override;

private:
	mutable GError* m_lastError{nullptr};
};

