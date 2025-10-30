#pragma once

class IObject;

class CEvent {
public:
	enum EType {
		MOUSE_DOWN,
		MOUSE_UP,
		MOUSE_MOVE,
		KEY_DOWN,
		KEY_UP,
		FOCUS_IN,
		FOCUS_OUT,
		VALUE_CHANGE,
		STATE_CHANGE
	};

	EType type;
	IObject* source;
	void* data;
};


