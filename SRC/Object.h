#pragma once

#include <vector>
#include <memory>
#include "Rect.h"

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
		LIST_ITEM,
		MENU,
		MENU_ITEM,
		SLIDER,
		PROGRESS_BAR,
		IMAGE,
		PANEL,
		WINDOW
	};

	enum EObjectState : unsigned int {
		NO = 0,
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

	IObject() = default;
	virtual ~IObject() = default;

	[[nodiscard]] static std::string GetTypeName(const EObjectType& type);
	[[nodiscard]] static std::vector<std::string> GetStateNames(const EObjectState& states, bool require_all = false);

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
};

[[nodiscard]] std::shared_ptr<IObject> GetDesktopObject();
[[nodiscard]] std::shared_ptr<IObject> FindFocusedObject(std::shared_ptr<IObject> start_from, bool force = false);

