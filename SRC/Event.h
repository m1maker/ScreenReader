#pragma once
#include <memory>
#include "Object.h"

class IEvent {
public:

	enum EEventType {
		NONE = 0,
		FOCUS_GAINED,
		FOCUS_LOST,
		CLICKED,
		VALUE_CHANGED,
		SELECTION_CHANGED,
		STATE_CHANGED,
		VISIBILITY_CHANGED,
		ENABLED_CHANGED,
		TEXT_CHANGED,
		CHILD_ADDED,
		CHILD_REMOVED,
		LAYOUT_UPDATED
	};

	EEventType type;

	IEvent() = default;
	virtual ~IEvent() = default;
};

class CObjectEvent : public IEvent {
public:
	std::shared_ptr<IObject> object;
};

