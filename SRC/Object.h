#pragma once

#include <vector>
#include <memory>
#include "Event.h"
#include "Rect.h"

class CEvent;

class IObject {
public:
	enum EObjectType : unsigned char {
		UNKNOWN = 0,
		BUTTON,
		TEXT_FIELD,
		LABEL,
		CHECKBOX,
		RADIO_BUTTON,
		COMBO_BOX,
		LIST_BOX,
		SLIDER,
		PROGRESS_BAR,
		IMAGE,
		PANEL,
		WINDOW
	};

	enum EObjectState : unsigned int {
		VISIBLE = 1 << 0,
		ENABLED = 1 << 1,
		FOCUSED = 1 << 2,
		SELECTED = 1 << 3,
		CHECKED = 1 << 4,
		EXPANDED = 1 << 5,
		READONLY = 1 << 6,
		REQUIRED = 1 << 7,
		INVALID = 1 << 8,
		HOVERED = 1 << 9,
		PRESSED = 1 << 10,
		DEFAULT = 1 << 11,
		LOADING = 1 << 12,
		COLLAPSED = 1 << 13
	};

	enum EEventType {
		EVENT_NONE = 0,
		EVENT_FOCUS_GAINED,
		EVENT_FOCUS_LOST,
		EVENT_CLICKED,
		EVENT_VALUE_CHANGED,
		EVENT_SELECTION_CHANGED,
		EVENT_STATE_CHANGED,
		EVENT_VISIBILITY_CHANGED,
		EVENT_ENABLED_CHANGED,
		EVENT_TEXT_CHANGED,
		EVENT_CHILD_ADDED,
		EVENT_CHILD_REMOVED,
		EVENT_LAYOUT_UPDATED
	};

	IObject() = default;

	virtual ~IObject() = default;

	[[nodiscard]] virtual EObjectType GetType() const = 0;
	[[nodiscard]] virtual bool IsVisible() const = 0;
	[[nodiscard]] virtual bool IsEnabled() const = 0;

	[[nodiscard]] virtual unsigned int GetState() const = 0;
	[[nodiscard]] virtual bool HasState(EObjectState state) const = 0;

	virtual bool HandleEvent(const CEvent& event) = 0;

	[[nodiscard]] virtual IObject* GetParent() const = 0;
	[[nodiscard]] virtual const std::vector<IObject*>& GetChildren() const = 0;

	[[nodiscard]] virtual struct SRect GetBounds() const = 0;

	[[nodiscard]] virtual int GetTabIndex() const = 0;

	[[nodiscard]] virtual std::string GetText() const = 0;

	[[nodiscard]] virtual IObject* GetSelectedItem() const = 0;
	[[nodiscard]] virtual const std::vector<IObject*>& GetItems() const = 0;

	[[nodiscard]] virtual double GetMinValue() const = 0;
	[[nodiscard]] virtual double GetMaxValue() const = 0;
	[[nodiscard]] virtual double GetCurrentValue() const = 0;

protected:
	IObject* m_parent{nullptr};
	std::vector<IObject*> m_children;
};

