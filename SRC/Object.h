// Object interface.
#pragma once
#include <vector>
#include <memory>
#include "Rect.h"
#include "Cache.h"

class IObject {
public:
	enum EObjectType : unsigned char {
		UNKNOWN = 0,
		BUTTON,
		TOGGLE_BUTTON,
		TEXT_FIELD,
		LABEL,
		CHECKBOX,
		RADIO_BUTTON,
		COMBO_BOX,
		LIST_BOX,
		LIST_ITEM,
		MENU,
		MENU_ITEM,
		SLIDER,
		PROGRESS_BAR,
		IMAGE,
		PANEL,
		WINDOW,
		DIALOG
	};

	enum EObjectState : unsigned long long {
		NO = 0,
		VISIBLE = 1 << 0,
		ENABLED = 1 << 1,
		FOCUSED = 1 << 2,
		SELECTED = 1 << 3,
		CHECKED = 1 << 4,
		EXPANDED = 1 << 5,
		READONLY = 1 << 6,
		MULTI_LINE = 1 << 7,
		SECURE = 1 << 8,
		REQUIRED = 1 << 9,
		INVALID = 1 << 10,
		HOVERED = 1 << 11,
		PRESSED = 1 << 12,
		DEFAULT = 1 << 13,
		LOADING = 1 << 14,
		COLLAPSED = 1 << 15,

		EDITABLE = 1 << 16,
		EXPANDABLE = 1 << 17,
		FOCUSABLE = 1 << 18,
		SELECTABLE = 1 << 19,
		MULTI_SELECTABLE = 1 << 20,
		RESIZABLE = 1 << 21,
		CHECKABLE = 1 << 22
	};

	IObject() = default;
	virtual ~IObject() = default;

	[[nodiscard]] static auto GetTypeName(const EObjectType& type, bool require_all = false) -> std::string;
	[[nodiscard]] static auto GetStateNames(const EObjectType& type, const unsigned long long& states, bool require_all = false) -> std::vector<std::string>;

	[[nodiscard]] static constexpr inline auto IsValidParent(const EObjectType& type) -> bool {
		switch (type) {
			case LIST_BOX:
			case MENU:
			case PANEL:
			case WINDOW:
			case DIALOG:
				return true;
		}

		return false;
	}

	[[nodiscard]] virtual auto GetNativeHandle()const noexcept -> void* = 0;

	[[nodiscard]] virtual auto IsValid()const noexcept -> bool = 0;

	[[nodiscard]] virtual auto GetType()const -> EObjectType = 0;
	[[nodiscard]] virtual auto IsVisible()const -> bool = 0;
	[[nodiscard]] virtual auto IsEnabled()const -> bool = 0;

	[[nodiscard]] virtual auto GetState()const -> unsigned long long = 0;
	[[nodiscard]] virtual auto HasState(EObjectState state)const -> bool = 0;

	[[nodiscard]] virtual auto GetParent()const -> std::weak_ptr<IObject> = 0;
	[[nodiscard]] virtual auto GetChildren()const -> const std::vector<std::shared_ptr<IObject>>& = 0;
	[[nodiscard]] virtual auto GetChildrenCount()const -> int = 0;

	[[nodiscard]] virtual auto GetBounds()const -> struct SRect = 0;

	[[nodiscard]] virtual auto GetIndex()const -> int = 0;

	[[nodiscard]] virtual auto GetApplicationName()const -> std::string = 0;

	[[nodiscard]] virtual auto GetName()const -> std::string = 0;
	[[nodiscard]] virtual auto GetDescription()const -> std::string = 0;

	[[nodiscard]] virtual auto GetCursor()const -> int = 0;
	[[nodiscard]] virtual auto GetText(bool at_cursor = false)const -> std::string = 0;

	[[nodiscard]] virtual auto GetMinValue()const -> double = 0;
	[[nodiscard]] virtual auto GetMaxValue()const -> double = 0;
	[[nodiscard]] virtual auto GetCurrentValue()const -> double = 0;

protected:
	DeclareCache(EObjectType, m_type);
	DeclareCache(unsigned long long, m_states);
	DeclareCache(std::weak_ptr<IObject>, m_parent);
	DeclareCache(std::vector<std::shared_ptr<IObject>>, m_children);
	DeclareCache(int, m_childrenCount);
	DeclareCache(int, m_index);
	DeclareCache(std::string, m_name);
	DeclareCache(std::string, m_applicationName);
	DeclareCache(std::string, m_description);
	DeclareCache(int, m_cursor);
	DeclareCache(double, m_minValue);
	DeclareCache(double, m_maxValue);
	DeclareCache(double, m_currentValue);
};

/*
Compare two objects.
*/
[[nodiscard]] inline auto ObjectIsSame(const std::shared_ptr<IObject>& obj1, const std::shared_ptr<IObject>& obj2) -> bool {
	if (!obj1 || !obj2) {
		return !obj1 && !obj2;
	}

	if (obj1.get() == obj2.get()) {
		return true;
	}

	if (obj1->GetType() != obj2->GetType()) {
		return false;
	}

	if (obj1->GetApplicationName() != obj2->GetApplicationName()) {
		return false;
	}

	if (obj1->GetName() != obj2->GetName()) {
		return false;
	}

	if (obj1->GetDescription() != obj2->GetDescription()) {
		return false;
	}

	if (obj1->GetText() != obj2->GetText()) {
		return false;
	}

	return true;
}

/*
Attention! GetDesktopObject is not implemented in
 Object.cpp.
This should be done by the platform implementation of IObject.
*/
[[nodiscard]] auto GetDesktopObject() -> std::shared_ptr<IObject>; // Root object.
[[nodiscard]] auto FindFocusedObject(std::shared_ptr<IObject> start_from, bool force = false) -> std::shared_ptr<IObject>;

[[nodiscard]] auto DumpObjectToString(const std::shared_ptr<IObject>& obj, int indent = 0, bool recursive = false, int max_depth = 3, int current_depth = 0) -> std::string;

