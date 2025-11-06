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
		LAYOUT_UPDATED,

		KEY_PRESSED,
		KEY_RELEASED
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
class CObjectEvent final : public IEvent {
public:
	std::shared_ptr<IObject> object;
};

/*
The keyboard event category.
Key pressed and released events. always dispatch a CKeyboardEvent.
*/
class CKeyboardEvent final : public IEvent {
public:

	enum EKeycode : unsigned short {
		KEY_A = 'A',
		KEY_B = 'B',
		KEY_C = 'C',
		KEY_D = 'D',
		KEY_E = 'E',
		KEY_F = 'F',
		KEY_G = 'G',
		KEY_H = 'H',
		KEY_I = 'I',
		KEY_J = 'J',
		KEY_K = 'K',
		KEY_L = 'L',
		KEY_M = 'M',
		KEY_N = 'N',
		KEY_O = 'O',
		KEY_P = 'P',
		KEY_Q = 'Q',
		KEY_R = 'R',
		KEY_S = 'S',
		KEY_T = 'T',
		KEY_U = 'U',
		KEY_V = 'V',
		KEY_W = 'W',
		KEY_X = 'X',
		KEY_Y = 'Y',
		KEY_Z = 'Z',

		KEY_0 = '0',
		KEY_1 = '1',
		KEY_2 = '2',
		KEY_3 = '3',
		KEY_4 = '4',
		KEY_5 = '5',
		KEY_6 = '6',
		KEY_7 = '7',
		KEY_8 = '8',
		KEY_9 = '9',

		KEY_F1 = 0x70,
		KEY_F2,
		KEY_F3,
		KEY_F4,
		KEY_F5,
		KEY_F6,
		KEY_F7,
		KEY_F8,
		KEY_F9,
		KEY_F10,
		KEY_F11,
		KEY_F12,
		KEY_F13,
		KEY_F14,
		KEY_F15,
		KEY_F16,
		KEY_F17,
		KEY_F18,
		KEY_F19,
		KEY_F20,
		KEY_F21,
		KEY_F22,
		KEY_F23,
		KEY_F24,

		KEY_UP = 0x26,
		KEY_DOWN = 0x28,
		KEY_LEFT = 0x25,
		KEY_RIGHT = 0x27,
		KEY_HOME = 0x24,
		KEY_END = 0x23,
		KEY_PAGE_UP = 0x21,
		KEY_PAGE_DOWN = 0x22,
		KEY_INSERT = 0x2D,
		KEY_DELETE = 0x2E,

		KEY_ESCAPE = 0x1B,
		KEY_TAB = 0x09,
		KEY_CAPS_LOCK = 0x14,
		KEY_SHIFT = 0x10,
		KEY_CTRL = 0x11,
		KEY_ALT = 0x12,
		KEY_SPACE = 0x20,
		KEY_ENTER = 0x0D,
		KEY_BACKSPACE = 0x08,

		KEY_LEFT_SHIFT = 0xA0,
		KEY_RIGHT_SHIFT = 0xA1,
		KEY_LEFT_CTRL = 0xA2,
		KEY_RIGHT_CTRL = 0xA3,
		KEY_LEFT_ALT = 0xA4,
		KEY_RIGHT_ALT = 0xA5,
		KEY_LEFT_SUPER = 0x5B,
		KEY_RIGHT_SUPER = 0x5C,
		KEY_MENU = 0x5D,

		KEY_NUMPAD_0 = 0x60,
		KEY_NUMPAD_1,
		KEY_NUMPAD_2,
		KEY_NUMPAD_3,
		KEY_NUMPAD_4,
		KEY_NUMPAD_5,
		KEY_NUMPAD_6,
		KEY_NUMPAD_7,
		KEY_NUMPAD_8,
		KEY_NUMPAD_9,
		KEY_NUMPAD_MULTIPLY,
		KEY_NUMPAD_ADD,
		KEY_NUMPAD_SEPARATOR,
		KEY_NUMPAD_SUBTRACT,
		KEY_NUMPAD_DECIMAL,
		KEY_NUMPAD_DIVIDE,
		KEY_NUMPAD_ENTER = 0xE0,

		KEY_SCROLL_LOCK = 0x91,
		KEY_NUM_LOCK = 0x90,
		KEY_PAUSE = 0x13,

		KEY_GRAVE = 0xC0,
		KEY_MINUS = 0xBD,
		KEY_EQUALS = 0xBB,
		KEY_LEFT_BRACKET = 0xDB,
		KEY_RIGHT_BRACKET = 0xDD,
		KEY_BACKSLASH = 0xDC,
		KEY_SEMICOLON = 0xBA,
		KEY_APOSTROPHE = 0xDE,
		KEY_COMMA = 0xBC,
		KEY_PERIOD = 0xBE,
		KEY_SLASH = 0xBF,

		KEY_VOLUME_MUTE = 0xAD,
		KEY_VOLUME_DOWN = 0xAE,
		KEY_VOLUME_UP = 0xAF,
		KEY_MEDIA_NEXT = 0xB0,
		KEY_MEDIA_PREV = 0xB1,
		KEY_MEDIA_STOP = 0xB2,
		KEY_MEDIA_PLAY_PAUSE = 0xB3,

		KEY_BROWSER_BACK = 0xA6,
		KEY_BROWSER_FORWARD = 0xA7,
		KEY_BROWSER_REFRESH = 0xA8,
		KEY_BROWSER_STOP = 0xA9,
		KEY_BROWSER_SEARCH = 0xAA,
		KEY_BROWSER_FAVORITES = 0xAB,
		KEY_BROWSER_HOME = 0xAC,

		KEY_LAUNCH_MAIL = 0xB4,
		KEY_LAUNCH_MEDIA_SELECT = 0xB5,
		KEY_LAUNCH_APP1 = 0xB6,
		KEY_LAUNCH_APP2 = 0xB7,

		KEY_MOUSE_LEFT = 0x01,
		KEY_MOUSE_RIGHT = 0x02,
		KEY_MOUSE_MIDDLE = 0x04,
		KEY_MOUSE_X1 = 0x05,
		KEY_MOUSE_X2 = 0x06,

		KEY_NONE = 0x00,
		KEY_ANY = 0xFFFF
	};

	enum EModifier : unsigned char {
		MODIFIER_NONE = 0,
		MODIFIER_SHIFT = 1 << 0,
		MODIFIER_CTRL = 1 << 1,
		MODIFIER_ALT = 1 << 2,
		MODIFIER_SUPER = 1 << 3,
		MODIFIER_CAPS_LOCK = 1 << 4,
		MODIFIER_NUM_LOCK = 1 << 5
	};

	EKeycode keycode;
	unsigned char modifiers;
};

