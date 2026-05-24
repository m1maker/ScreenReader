// AT_SPI's event listener.
module;
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
export module Platforms.Linux.EventListener;
import Core.Device;
import Core.Event;
import Core.KeyInfo;
import Core.Logger;
import Core.Object;
import Traits.EventListener;

/*
AT-SPI has a listener where you need to register the required events one by one.
But there's one very strange thing: all of these types are strings. Look at this code.
*/
struct SEventTypeMapEntry final {
	std::string_view key;
	EObjectEventType value;
};

constexpr std::array cAtspiObjectEventTypeMap = {
	SEventTypeMapEntry{"object:state-changed:focused", EObjectEventType::FOCUS_GAINED},
	SEventTypeMapEntry{"object:state-changed:checked", EObjectEventType::STATE_CHANGED},
	SEventTypeMapEntry{"object:selection-changed", EObjectEventType::SELECTION_CHANGED},
	SEventTypeMapEntry{"object:text-selection-changed", EObjectEventType::TEXT_SELECTION_CHANGED},
	SEventTypeMapEntry{"object:property-change:accessible-name", EObjectEventType::NAME_CHANGED},
	SEventTypeMapEntry{"object:property-change:accessible-description", EObjectEventType::DESCRIPTION_CHANGED},
	SEventTypeMapEntry{"object:property-change:accessible-value", EObjectEventType::VALUE_CHANGED},
	SEventTypeMapEntry{"object:text-caret-moved", EObjectEventType::CURSOR_MOVED}};

[[nodiscard]] constexpr inline auto GetEventTypeFromString(gchar* type) -> EObjectEventType {
	if (!type) [[unlikely]]
		return EObjectEventType::NONE;

	const std::string_view type_view(type);
	for (const auto& entry : cAtspiObjectEventTypeMap) {
		if (type_view == entry.key)
			return entry.value;
	}
	return EObjectEventType::NONE;
}

[[nodiscard]] constexpr inline auto LinuxKeycodeToKeyboardEventKeycode(uint16_t linux_keycode) -> EKeycode {
	switch (linux_keycode) {
	case KEY_A:
		return KEYCODE_A;
	case KEY_B:
		return KEYCODE_B;
	case KEY_C:
		return KEYCODE_C;
	case KEY_D:
		return KEYCODE_D;
	case KEY_E:
		return KEYCODE_E;
	case KEY_F:
		return KEYCODE_F;
	case KEY_G:
		return KEYCODE_G;
	case KEY_H:
		return KEYCODE_H;
	case KEY_I:
		return KEYCODE_I;
	case KEY_J:
		return KEYCODE_J;
	case KEY_K:
		return KEYCODE_K;
	case KEY_L:
		return KEYCODE_L;
	case KEY_M:
		return KEYCODE_M;
	case KEY_N:
		return KEYCODE_N;
	case KEY_O:
		return KEYCODE_O;
	case KEY_P:
		return KEYCODE_P;
	case KEY_Q:
		return KEYCODE_Q;
	case KEY_R:
		return KEYCODE_R;
	case KEY_S:
		return KEYCODE_S;
	case KEY_T:
		return KEYCODE_T;
	case KEY_U:
		return KEYCODE_U;
	case KEY_V:
		return KEYCODE_V;
	case KEY_W:
		return KEYCODE_W;
	case KEY_X:
		return KEYCODE_X;
	case KEY_Y:
		return KEYCODE_Y;
	case KEY_Z:
		return KEYCODE_Z;

	case KEY_0:
		return KEYCODE_0;
	case KEY_1:
		return KEYCODE_1;
	case KEY_2:
		return KEYCODE_2;
	case KEY_3:
		return KEYCODE_3;
	case KEY_4:
		return KEYCODE_4;
	case KEY_5:
		return KEYCODE_5;
	case KEY_6:
		return KEYCODE_6;
	case KEY_7:
		return KEYCODE_7;
	case KEY_8:
		return KEYCODE_8;
	case KEY_9:
		return KEYCODE_9;

	case KEY_F1:
		return KEYCODE_F1;
	case KEY_F2:
		return KEYCODE_F2;
	case KEY_F3:
		return KEYCODE_F3;
	case KEY_F4:
		return KEYCODE_F4;
	case KEY_F5:
		return KEYCODE_F5;
	case KEY_F6:
		return KEYCODE_F6;
	case KEY_F7:
		return KEYCODE_F7;
	case KEY_F8:
		return KEYCODE_F8;
	case KEY_F9:
		return KEYCODE_F9;
	case KEY_F10:
		return KEYCODE_F10;
	case KEY_F11:
		return KEYCODE_F11;
	case KEY_F12:
		return KEYCODE_F12;
	case KEY_F13:
		return KEYCODE_F13;
	case KEY_F14:
		return KEYCODE_F14;
	case KEY_F15:
		return KEYCODE_F15;
	case KEY_F16:
		return KEYCODE_F16;
	case KEY_F17:
		return KEYCODE_F17;
	case KEY_F18:
		return KEYCODE_F18;
	case KEY_F19:
		return KEYCODE_F19;
	case KEY_F20:
		return KEYCODE_F20;
	case KEY_F21:
		return KEYCODE_F21;
	case KEY_F22:
		return KEYCODE_F22;
	case KEY_F23:
		return KEYCODE_F23;
	case KEY_F24:
		return KEYCODE_F24;

	case KEY_UP:
		return KEYCODE_UP;
	case KEY_DOWN:
		return KEYCODE_DOWN;
	case KEY_LEFT:
		return KEYCODE_LEFT;
	case KEY_RIGHT:
		return KEYCODE_RIGHT;
	case KEY_HOME:
		return KEYCODE_HOME;
	case KEY_END:
		return KEYCODE_END;
	case KEY_PAGEUP:
		return KEYCODE_PAGE_UP;
	case KEY_PAGEDOWN:
		return KEYCODE_PAGE_DOWN;
	case KEY_INSERT:
		return KEYCODE_INSERT;
	case KEY_DELETE:
		return KEYCODE_DELETE;

	case KEY_ESC:
		return KEYCODE_ESCAPE;
	case KEY_TAB:
		return KEYCODE_TAB;
	case KEY_CAPSLOCK:
		return KEYCODE_CAPS_LOCK;
	case KEY_SPACE:
		return KEYCODE_SPACE;
	case KEY_ENTER:
		return KEYCODE_ENTER;
	case KEY_BACKSPACE:
		return KEYCODE_BACKSPACE;

	case KEY_LEFTSHIFT:
		return KEYCODE_LEFT_SHIFT;
	case KEY_RIGHTSHIFT:
		return KEYCODE_RIGHT_SHIFT;
	case KEY_LEFTCTRL:
		return KEYCODE_LEFT_CTRL;
	case KEY_RIGHTCTRL:
		return KEYCODE_RIGHT_CTRL;
	case KEY_LEFTALT:
		return KEYCODE_LEFT_ALT;
	case KEY_RIGHTALT:
		return KEYCODE_RIGHT_ALT;
	case KEY_LEFTMETA:
		return KEYCODE_LEFT_SUPER;
	case KEY_RIGHTMETA:
		return KEYCODE_RIGHT_SUPER;
	case KEY_COMPOSE:
		return KEYCODE_MENU;

	case KEY_KP0:
		return KEYCODE_NUMPAD_0;
	case KEY_KP1:
		return KEYCODE_NUMPAD_1;
	case KEY_KP2:
		return KEYCODE_NUMPAD_2;
	case KEY_KP3:
		return KEYCODE_NUMPAD_3;
	case KEY_KP4:
		return KEYCODE_NUMPAD_4;
	case KEY_KP5:
		return KEYCODE_NUMPAD_5;
	case KEY_KP6:
		return KEYCODE_NUMPAD_6;
	case KEY_KP7:
		return KEYCODE_NUMPAD_7;
	case KEY_KP8:
		return KEYCODE_NUMPAD_8;
	case KEY_KP9:
		return KEYCODE_NUMPAD_9;
	case KEY_KPASTERISK:
		return KEYCODE_NUMPAD_MULTIPLY;
	case KEY_KPPLUS:
		return KEYCODE_NUMPAD_ADD;
	case KEY_KPMINUS:
		return KEYCODE_NUMPAD_SUBTRACT;
	case KEY_KPDOT:
		return KEYCODE_NUMPAD_DECIMAL;
	case KEY_KPSLASH:
		return KEYCODE_NUMPAD_DIVIDE;
	case KEY_KPENTER:
		return KEYCODE_NUMPAD_ENTER;
	case KEY_KPCOMMA:
		return KEYCODE_NUMPAD_SEPARATOR;

	case KEY_SCROLLLOCK:
		return KEYCODE_SCROLL_LOCK;
	case KEY_NUMLOCK:
		return KEYCODE_NUM_LOCK;
	case KEY_PAUSE:
		return KEYCODE_PAUSE;

	case KEY_GRAVE:
		return KEYCODE_GRAVE;
	case KEY_MINUS:
		return KEYCODE_MINUS;
	case KEY_EQUAL:
		return KEYCODE_EQUALS;
	case KEY_LEFTBRACE:
		return KEYCODE_LEFT_BRACKET;
	case KEY_RIGHTBRACE:
		return KEYCODE_RIGHT_BRACKET;
	case KEY_BACKSLASH:
		return KEYCODE_BACKSLASH;
	case KEY_SEMICOLON:
		return KEYCODE_SEMICOLON;
	case KEY_APOSTROPHE:
		return KEYCODE_APOSTROPHE;
	case KEY_COMMA:
		return KEYCODE_COMMA;
	case KEY_DOT:
		return KEYCODE_PERIOD;
	case KEY_SLASH:
		return KEYCODE_SLASH;

	case KEY_MUTE:
		return KEYCODE_VOLUME_MUTE;
	case KEY_VOLUMEDOWN:
		return KEYCODE_VOLUME_DOWN;
	case KEY_VOLUMEUP:
		return KEYCODE_VOLUME_UP;
	case KEY_NEXTSONG:
		return KEYCODE_MEDIA_NEXT;
	case KEY_PREVIOUSSONG:
		return KEYCODE_MEDIA_PREV;
	case KEY_STOPCD:
		return KEYCODE_MEDIA_STOP;
	case KEY_PLAYPAUSE:
		return KEYCODE_MEDIA_PLAY_PAUSE;

	case KEY_BACK:
		return KEYCODE_BROWSER_BACK;
	case KEY_FORWARD:
		return KEYCODE_BROWSER_FORWARD;
	case KEY_REFRESH:
		return KEYCODE_BROWSER_REFRESH;
	case KEY_STOP:
		return KEYCODE_BROWSER_STOP;
	case KEY_SEARCH:
		return KEYCODE_BROWSER_SEARCH;
	case KEY_BOOKMARKS:
		return KEYCODE_BROWSER_FAVORITES;
	case KEY_HOMEPAGE:
		return KEYCODE_BROWSER_HOME;

	case KEY_MAIL:
		return KEYCODE_LAUNCH_MAIL;
	case KEY_CALC:
		return KEYCODE_LAUNCH_APP1;
	case KEY_COMPUTER:
		return KEYCODE_LAUNCH_APP2;
	case KEY_MEDIA:
		return KEYCODE_LAUNCH_MEDIA_SELECT;

	case BTN_LEFT:
		return KEYCODE_MOUSE_LEFT;
	case BTN_RIGHT:
		return KEYCODE_MOUSE_RIGHT;
	case BTN_MIDDLE:
		return KEYCODE_MOUSE_MIDDLE;
	case BTN_SIDE:
		return KEYCODE_MOUSE_X1;
	case BTN_EXTRA:
		return KEYCODE_MOUSE_X2;

	default:
		return KEYCODE_NONE;
	}
}

export class CEventListenerAtspi final : public TEventListener<CEventListenerAtspi>, TModule<"EventListenerAtspi"> {
	AtspiEventListener* m_objectEventListener{nullptr};

	std::jthread m_keyboardListenerThread;
	void StartEvdevWatcher();
	void StopEvdevWatcher();

	[[nodiscard]] static auto FindKeyboardDevice() -> std::string;
	static void OnObjectEventCallback(AtspiEvent* event, void* user_data);

public:
	explicit CEventListenerAtspi();
	~CEventListenerAtspi();
	void do_ListenDevice(EDeviceType device, bool listen = true);

	void do_PushToMainThread(ThreadFunction function, void* pUserData);
};

export using CEventListener = CEventListenerAtspi;
