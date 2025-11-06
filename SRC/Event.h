// An interface for abstracting event types. Categories are implemented here.
#pragma once
#include <memory>
#include "Object.h"

class IEvent {
public:

	enum EEventType : unsigned char {
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
		CURSOR_MOVED,
		CHILD_ADDED,
		CHILD_REMOVED,
		PARENT_UPDATED,
		LAYOUT_UPDATED
	};

	EEventType type{NONE};
	bool now{false}; // A very specific flag. I'll describe it in the handlers.

	IEvent() = default;
	virtual ~IEvent() = default; // C++ requires an interface to have at least one virtual method.
};

/*
The object event category.
Events such as changes to an object's focus, state, name, description, value, etc., always dispatch a CObjectEvent.
*/
class CObjectEvent : public IEvent {
public:
	std::shared_ptr<IObject> object;
};

