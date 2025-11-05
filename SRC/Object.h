// Object interface.
#pragma once
#include <vector>
#include <memory>
#include "Rect.h"

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

	[[nodiscard]] static std::string GetTypeName(const EObjectType& type, bool require_all = false);
	[[nodiscard]] static std::vector<std::string> GetStateNames(const EObjectType& type, const EObjectState& states, bool require_all = false);

	[[nodiscard]] static constexpr inline bool IsValidParent(const EObjectType& type) {
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

	[[nodiscard]] virtual void* GetNativeHandle() = 0;

	[[nodiscard]] virtual bool IsValid() = 0;

	[[nodiscard]] virtual EObjectType GetType() = 0;
	[[nodiscard]] virtual bool IsVisible() = 0;
	[[nodiscard]] virtual bool IsEnabled() = 0;

	[[nodiscard]] virtual EObjectState GetState() = 0;
	[[nodiscard]] virtual bool HasState(EObjectState state) = 0;

	[[nodiscard]] virtual std::weak_ptr<IObject> GetParent() = 0;
	[[nodiscard]] virtual const std::vector<std::shared_ptr<IObject>>& GetChildren() = 0;

	[[nodiscard]] virtual struct SRect GetBounds() = 0;

	[[nodiscard]] virtual int GetTabIndex() = 0;

	[[nodiscard]] virtual std::string GetApplicationName() = 0;

	[[nodiscard]] virtual std::string GetName() = 0;
	[[nodiscard]] virtual std::string GetDescription() = 0;
	[[nodiscard]] virtual std::string GetText() = 0;

	[[nodiscard]] virtual double GetMinValue() = 0;
	[[nodiscard]] virtual double GetMaxValue() = 0;
	[[nodiscard]] virtual double GetCurrentValue() = 0;

protected:
	std::weak_ptr<IObject> m_parent;
	std::vector<std::shared_ptr<IObject>> m_children;
	/*
	This is an internal state; it may not be used in practice, but it simply exists.
	For example, it was needed with AT-SPI.
	AT-SPI has a password text object type, while IObject simply has a text field and a secure state.
	We simply check within the AT-SPI implementation if the type is password text and add the secure state.
	*/
	EObjectState m_states{NO};
};

/*
Compare two objects to avoid announcement duplicates.
I don't know yet how much this will slow down the screen reader, but I hope it won't.

For some reason, AT-SPI can send the same object to two focus events, so we'll throw such events in the event handler.
*/
[[nodiscard]] inline bool ObjectIsSame(const std::shared_ptr<IObject>& obj1, const std::shared_ptr<IObject>& obj2) {
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
[[nodiscard]] std::shared_ptr<IObject> GetDesktopObject(); // Root object.
[[nodiscard]] std::shared_ptr<IObject> FindFocusedObject(std::shared_ptr<IObject> start_from, bool force = false);

