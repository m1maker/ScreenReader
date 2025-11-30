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

	[[nodiscard]] static std::string GetTypeName(const EObjectType& type, bool require_all = false);
	[[nodiscard]] static std::vector<std::string> GetStateNames(const EObjectType& type, const unsigned long long& states, bool require_all = false);

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

	[[nodiscard]] virtual void* GetNativeHandle()const noexcept = 0;

	[[nodiscard]] virtual bool IsValid()const noexcept = 0;

	[[nodiscard]] virtual EObjectType GetType()const noexcept = 0;
	[[nodiscard]] virtual bool IsVisible()const noexcept = 0;
	[[nodiscard]] virtual bool IsEnabled()const noexcept = 0;

	[[nodiscard]] virtual unsigned long long GetState()const noexcept = 0;
	[[nodiscard]] virtual bool HasState(EObjectState state)const noexcept = 0;

	[[nodiscard]] virtual std::weak_ptr<IObject> GetParent()const noexcept = 0;
	[[nodiscard]] virtual const std::vector<std::shared_ptr<IObject>>& GetChildren()const noexcept = 0;

	[[nodiscard]] virtual struct SRect GetBounds()const noexcept = 0;

	[[nodiscard]] virtual int GetTabIndex()const noexcept = 0;

	[[nodiscard]] virtual std::string GetApplicationName()const noexcept = 0;

	[[nodiscard]] virtual std::string GetName()const noexcept = 0;
	[[nodiscard]] virtual std::string GetDescription()const noexcept = 0;

	[[nodiscard]] virtual int GetCursor()const noexcept = 0;
	[[nodiscard]] virtual std::string GetText(bool at_cursor = false)const noexcept = 0;

	[[nodiscard]] virtual double GetMinValue()const noexcept = 0;
	[[nodiscard]] virtual double GetMaxValue()const noexcept = 0;
	[[nodiscard]] virtual double GetCurrentValue()const noexcept = 0;

protected:
	DeclareCache(EObjectType, m_type);
	DeclareCache(unsigned long long, m_states);
	DeclareCache(std::weak_ptr<IObject>, m_parent);
	DeclareCache(std::vector<std::shared_ptr<IObject>>, m_children);
	DeclareCache(std::string, m_name);
	DeclareCache(std::string, m_applicationName);
	DeclareCache(std::string, m_description);
	DeclareCache(int, m_cursor);
	DeclareCache(double, m_minValue);
	DeclareCache(double, m_maxValue);
	DeclareCache(double, m_currentValue);
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

