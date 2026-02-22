// AT_SPI's event listener.
#pragma once
#include "ObjectAtspi.h"

#include <Core/Device.h>
#include <Traits/EventListener.h>
#include <Traits/RefCountedObject.h>
#include <array>
#include <atomic>
#include <atspi/atspi.h>
#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <memory>
#include <string>
#include <thread>
#include <unistd.h>
#include <unordered_map>

/*
AT-SPI has a listener where you need to register the required events one by one.
But there's one very strange thing: all of these types are strings. Look at this code.
*/
inline const std::unordered_map<std::string_view, CObjectEvent::EObjectEventType> cAtspiObjectEventTypeMap = {
	{"object:state-changed:focused", CObjectEvent::FOCUS_GAINED},
	{"object:state-changed", CObjectEvent::STATE_CHANGED},
	{"object:selection-changed", CObjectEvent::SELECTION_CHANGED},
	{"object:text-selection-changed", CObjectEvent::SELECTION_CHANGED},
	{"object:property-change:accessible-value", CObjectEvent::VALUE_CHANGED},
	{"object:text-caret-moved", CObjectEvent::CURSOR_MOVED}};

[[nodiscard]] constexpr inline auto GetEventTypeFromString(gchar* type) -> CObjectEvent::EObjectEventType {
	if (!type) [[unlikely]]
		return CObjectEvent::NONE;

	std::string_view type_str(type);

	auto it = cAtspiObjectEventTypeMap.find(type_str);
	if (it != cAtspiObjectEventTypeMap.end()) [[likely]] {
		return it->second;
	}

	return CObjectEvent::NONE;
}

[[nodiscard]] constexpr inline auto LinuxKeycodeToKeyboardEventKeycode(const uint16_t& linux_keycode)
	-> CKeyboardEvent::EKeycode {
	switch (linux_keycode) {
	case KEY_A:
		return CKeyboardEvent::KEYCODE_A;
	case KEY_B:
		return CKeyboardEvent::KEYCODE_B;
	case KEY_C:
		return CKeyboardEvent::KEYCODE_C;
	case KEY_D:
		return CKeyboardEvent::KEYCODE_D;
	case KEY_E:
		return CKeyboardEvent::KEYCODE_E;
	case KEY_F:
		return CKeyboardEvent::KEYCODE_F;
	case KEY_G:
		return CKeyboardEvent::KEYCODE_G;
	case KEY_H:
		return CKeyboardEvent::KEYCODE_H;
	case KEY_I:
		return CKeyboardEvent::KEYCODE_I;
	case KEY_J:
		return CKeyboardEvent::KEYCODE_J;
	case KEY_K:
		return CKeyboardEvent::KEYCODE_K;
	case KEY_L:
		return CKeyboardEvent::KEYCODE_L;
	case KEY_M:
		return CKeyboardEvent::KEYCODE_M;
	case KEY_N:
		return CKeyboardEvent::KEYCODE_N;
	case KEY_O:
		return CKeyboardEvent::KEYCODE_O;
	case KEY_P:
		return CKeyboardEvent::KEYCODE_P;
	case KEY_Q:
		return CKeyboardEvent::KEYCODE_Q;
	case KEY_R:
		return CKeyboardEvent::KEYCODE_R;
	case KEY_S:
		return CKeyboardEvent::KEYCODE_S;
	case KEY_T:
		return CKeyboardEvent::KEYCODE_T;
	case KEY_U:
		return CKeyboardEvent::KEYCODE_U;
	case KEY_V:
		return CKeyboardEvent::KEYCODE_V;
	case KEY_W:
		return CKeyboardEvent::KEYCODE_W;
	case KEY_X:
		return CKeyboardEvent::KEYCODE_X;
	case KEY_Y:
		return CKeyboardEvent::KEYCODE_Y;
	case KEY_Z:
		return CKeyboardEvent::KEYCODE_Z;

	case KEY_0:
		return CKeyboardEvent::KEYCODE_0;
	case KEY_1:
		return CKeyboardEvent::KEYCODE_1;
	case KEY_2:
		return CKeyboardEvent::KEYCODE_2;
	case KEY_3:
		return CKeyboardEvent::KEYCODE_3;
	case KEY_4:
		return CKeyboardEvent::KEYCODE_4;
	case KEY_5:
		return CKeyboardEvent::KEYCODE_5;
	case KEY_6:
		return CKeyboardEvent::KEYCODE_6;
	case KEY_7:
		return CKeyboardEvent::KEYCODE_7;
	case KEY_8:
		return CKeyboardEvent::KEYCODE_8;
	case KEY_9:
		return CKeyboardEvent::KEYCODE_9;

	case KEY_F1:
		return CKeyboardEvent::KEYCODE_F1;
	case KEY_F2:
		return CKeyboardEvent::KEYCODE_F2;
	case KEY_F3:
		return CKeyboardEvent::KEYCODE_F3;
	case KEY_F4:
		return CKeyboardEvent::KEYCODE_F4;
	case KEY_F5:
		return CKeyboardEvent::KEYCODE_F5;
	case KEY_F6:
		return CKeyboardEvent::KEYCODE_F6;
	case KEY_F7:
		return CKeyboardEvent::KEYCODE_F7;
	case KEY_F8:
		return CKeyboardEvent::KEYCODE_F8;
	case KEY_F9:
		return CKeyboardEvent::KEYCODE_F9;
	case KEY_F10:
		return CKeyboardEvent::KEYCODE_F10;
	case KEY_F11:
		return CKeyboardEvent::KEYCODE_F11;
	case KEY_F12:
		return CKeyboardEvent::KEYCODE_F12;
	case KEY_F13:
		return CKeyboardEvent::KEYCODE_F13;
	case KEY_F14:
		return CKeyboardEvent::KEYCODE_F14;
	case KEY_F15:
		return CKeyboardEvent::KEYCODE_F15;
	case KEY_F16:
		return CKeyboardEvent::KEYCODE_F16;
	case KEY_F17:
		return CKeyboardEvent::KEYCODE_F17;
	case KEY_F18:
		return CKeyboardEvent::KEYCODE_F18;
	case KEY_F19:
		return CKeyboardEvent::KEYCODE_F19;
	case KEY_F20:
		return CKeyboardEvent::KEYCODE_F20;
	case KEY_F21:
		return CKeyboardEvent::KEYCODE_F21;
	case KEY_F22:
		return CKeyboardEvent::KEYCODE_F22;
	case KEY_F23:
		return CKeyboardEvent::KEYCODE_F23;
	case KEY_F24:
		return CKeyboardEvent::KEYCODE_F24;

	case KEY_UP:
		return CKeyboardEvent::KEYCODE_UP;
	case KEY_DOWN:
		return CKeyboardEvent::KEYCODE_DOWN;
	case KEY_LEFT:
		return CKeyboardEvent::KEYCODE_LEFT;
	case KEY_RIGHT:
		return CKeyboardEvent::KEYCODE_RIGHT;
	case KEY_HOME:
		return CKeyboardEvent::KEYCODE_HOME;
	case KEY_END:
		return CKeyboardEvent::KEYCODE_END;
	case KEY_PAGEUP:
		return CKeyboardEvent::KEYCODE_PAGE_UP;
	case KEY_PAGEDOWN:
		return CKeyboardEvent::KEYCODE_PAGE_DOWN;
	case KEY_INSERT:
		return CKeyboardEvent::KEYCODE_INSERT;
	case KEY_DELETE:
		return CKeyboardEvent::KEYCODE_DELETE;

	case KEY_ESC:
		return CKeyboardEvent::KEYCODE_ESCAPE;
	case KEY_TAB:
		return CKeyboardEvent::KEYCODE_TAB;
	case KEY_CAPSLOCK:
		return CKeyboardEvent::KEYCODE_CAPS_LOCK;
	case KEY_SPACE:
		return CKeyboardEvent::KEYCODE_SPACE;
	case KEY_ENTER:
		return CKeyboardEvent::KEYCODE_ENTER;
	case KEY_BACKSPACE:
		return CKeyboardEvent::KEYCODE_BACKSPACE;

	case KEY_LEFTSHIFT:
		return CKeyboardEvent::KEYCODE_LEFT_SHIFT;
	case KEY_RIGHTSHIFT:
		return CKeyboardEvent::KEYCODE_RIGHT_SHIFT;
	case KEY_LEFTCTRL:
		return CKeyboardEvent::KEYCODE_LEFT_CTRL;
	case KEY_RIGHTCTRL:
		return CKeyboardEvent::KEYCODE_RIGHT_CTRL;
	case KEY_LEFTALT:
		return CKeyboardEvent::KEYCODE_LEFT_ALT;
	case KEY_RIGHTALT:
		return CKeyboardEvent::KEYCODE_RIGHT_ALT;
	case KEY_LEFTMETA:
		return CKeyboardEvent::KEYCODE_LEFT_SUPER;
	case KEY_RIGHTMETA:
		return CKeyboardEvent::KEYCODE_RIGHT_SUPER;
	case KEY_COMPOSE:
		return CKeyboardEvent::KEYCODE_MENU;

	case KEY_KP0:
		return CKeyboardEvent::KEYCODE_NUMPAD_0;
	case KEY_KP1:
		return CKeyboardEvent::KEYCODE_NUMPAD_1;
	case KEY_KP2:
		return CKeyboardEvent::KEYCODE_NUMPAD_2;
	case KEY_KP3:
		return CKeyboardEvent::KEYCODE_NUMPAD_3;
	case KEY_KP4:
		return CKeyboardEvent::KEYCODE_NUMPAD_4;
	case KEY_KP5:
		return CKeyboardEvent::KEYCODE_NUMPAD_5;
	case KEY_KP6:
		return CKeyboardEvent::KEYCODE_NUMPAD_6;
	case KEY_KP7:
		return CKeyboardEvent::KEYCODE_NUMPAD_7;
	case KEY_KP8:
		return CKeyboardEvent::KEYCODE_NUMPAD_8;
	case KEY_KP9:
		return CKeyboardEvent::KEYCODE_NUMPAD_9;
	case KEY_KPASTERISK:
		return CKeyboardEvent::KEYCODE_NUMPAD_MULTIPLY;
	case KEY_KPPLUS:
		return CKeyboardEvent::KEYCODE_NUMPAD_ADD;
	case KEY_KPMINUS:
		return CKeyboardEvent::KEYCODE_NUMPAD_SUBTRACT;
	case KEY_KPDOT:
		return CKeyboardEvent::KEYCODE_NUMPAD_DECIMAL;
	case KEY_KPSLASH:
		return CKeyboardEvent::KEYCODE_NUMPAD_DIVIDE;
	case KEY_KPENTER:
		return CKeyboardEvent::KEYCODE_NUMPAD_ENTER;
	case KEY_KPCOMMA:
		return CKeyboardEvent::KEYCODE_NUMPAD_SEPARATOR;

	case KEY_SCROLLLOCK:
		return CKeyboardEvent::KEYCODE_SCROLL_LOCK;
	case KEY_NUMLOCK:
		return CKeyboardEvent::KEYCODE_NUM_LOCK;
	case KEY_PAUSE:
		return CKeyboardEvent::KEYCODE_PAUSE;

	case KEY_GRAVE:
		return CKeyboardEvent::KEYCODE_GRAVE;
	case KEY_MINUS:
		return CKeyboardEvent::KEYCODE_MINUS;
	case KEY_EQUAL:
		return CKeyboardEvent::KEYCODE_EQUALS;
	case KEY_LEFTBRACE:
		return CKeyboardEvent::KEYCODE_LEFT_BRACKET;
	case KEY_RIGHTBRACE:
		return CKeyboardEvent::KEYCODE_RIGHT_BRACKET;
	case KEY_BACKSLASH:
		return CKeyboardEvent::KEYCODE_BACKSLASH;
	case KEY_SEMICOLON:
		return CKeyboardEvent::KEYCODE_SEMICOLON;
	case KEY_APOSTROPHE:
		return CKeyboardEvent::KEYCODE_APOSTROPHE;
	case KEY_COMMA:
		return CKeyboardEvent::KEYCODE_COMMA;
	case KEY_DOT:
		return CKeyboardEvent::KEYCODE_PERIOD;
	case KEY_SLASH:
		return CKeyboardEvent::KEYCODE_SLASH;

	case KEY_MUTE:
		return CKeyboardEvent::KEYCODE_VOLUME_MUTE;
	case KEY_VOLUMEDOWN:
		return CKeyboardEvent::KEYCODE_VOLUME_DOWN;
	case KEY_VOLUMEUP:
		return CKeyboardEvent::KEYCODE_VOLUME_UP;
	case KEY_NEXTSONG:
		return CKeyboardEvent::KEYCODE_MEDIA_NEXT;
	case KEY_PREVIOUSSONG:
		return CKeyboardEvent::KEYCODE_MEDIA_PREV;
	case KEY_STOPCD:
		return CKeyboardEvent::KEYCODE_MEDIA_STOP;
	case KEY_PLAYPAUSE:
		return CKeyboardEvent::KEYCODE_MEDIA_PLAY_PAUSE;

	case KEY_BACK:
		return CKeyboardEvent::KEYCODE_BROWSER_BACK;
	case KEY_FORWARD:
		return CKeyboardEvent::KEYCODE_BROWSER_FORWARD;
	case KEY_REFRESH:
		return CKeyboardEvent::KEYCODE_BROWSER_REFRESH;
	case KEY_STOP:
		return CKeyboardEvent::KEYCODE_BROWSER_STOP;
	case KEY_SEARCH:
		return CKeyboardEvent::KEYCODE_BROWSER_SEARCH;
	case KEY_BOOKMARKS:
		return CKeyboardEvent::KEYCODE_BROWSER_FAVORITES;
	case KEY_HOMEPAGE:
		return CKeyboardEvent::KEYCODE_BROWSER_HOME;

	case KEY_MAIL:
		return CKeyboardEvent::KEYCODE_LAUNCH_MAIL;
	case KEY_CALC:
		return CKeyboardEvent::KEYCODE_LAUNCH_APP1;
	case KEY_COMPUTER:
		return CKeyboardEvent::KEYCODE_LAUNCH_APP2;
	case KEY_MEDIA:
		return CKeyboardEvent::KEYCODE_LAUNCH_MEDIA_SELECT;

	case BTN_LEFT:
		return CKeyboardEvent::KEYCODE_MOUSE_LEFT;
	case BTN_RIGHT:
		return CKeyboardEvent::KEYCODE_MOUSE_RIGHT;
	case BTN_MIDDLE:
		return CKeyboardEvent::KEYCODE_MOUSE_MIDDLE;
	case BTN_SIDE:
		return CKeyboardEvent::KEYCODE_MOUSE_X1;
	case BTN_EXTRA:
		return CKeyboardEvent::KEYCODE_MOUSE_X2;

	default:
		return CKeyboardEvent::KEYCODE_NONE;
	}
}

[[nodiscard]] constexpr inline auto LinuxModifierToKeyboardEventModifier(const uint16_t& linux_keycode)
	-> CKeyboardEvent::EModifier {
	switch (linux_keycode) {
	case KEY_LEFTSHIFT:
	case KEY_RIGHTSHIFT:
		return CKeyboardEvent::MODIFIER_SHIFT;

	case KEY_LEFTCTRL:
	case KEY_RIGHTCTRL:
		return CKeyboardEvent::MODIFIER_CTRL;

	case KEY_LEFTALT:
	case KEY_RIGHTALT:
		return CKeyboardEvent::MODIFIER_ALT;

	case KEY_LEFTMETA:
	case KEY_RIGHTMETA:
		return CKeyboardEvent::MODIFIER_SUPER;

	case KEY_CAPSLOCK:
		return CKeyboardEvent::MODIFIER_CAPS_LOCK;

	case KEY_NUMLOCK:
		return CKeyboardEvent::MODIFIER_NUM_LOCK;

	case KEY_INSERT:
		return CKeyboardEvent::MODIFIER_INSERT;

	default:
		return CKeyboardEvent::MODIFIER_NONE;
	}
}

class CEventListenerAtspi final : public TEventListener<CEventListenerAtspi> {
	friend class CUinputDevice;
	AtspiEventListener* m_objectEventListener{nullptr};

	std::jthread m_keyboardListenerThread;
	std::atomic<bool> m_listenKeyboard{false};
	void StartEvdevWatcher();
	void StopEvdevWatcher();

	[[nodiscard]] static auto FindKeyboardDevice() -> std::string;

	[[nodiscard]] static auto ElevatePrivileges() -> bool;
	static void OnObjectEventCallback(AtspiEvent* event, void* user_data);

public:
	explicit CEventListenerAtspi();
	~CEventListenerAtspi();
	void do_ListenDevice(EDeviceType device, bool listen = true);

	void do_PushToMainThread(ThreadFunction function, void* pUserData);
};

using CEventListener = CEventListenerAtspi;
