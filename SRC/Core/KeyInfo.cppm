module;
#include <cstdint>
#include <string_view>
export module Core.KeyInfo;

export enum EKeycode : unsigned char {
	KEYCODE_NONE = 0,
	KEYCODE_ANY = 1,

	KEYCODE_A = 2,
	KEYCODE_B,
	KEYCODE_C,
	KEYCODE_D,
	KEYCODE_E,
	KEYCODE_F,
	KEYCODE_G,
	KEYCODE_H,
	KEYCODE_I,
	KEYCODE_J,
	KEYCODE_K,
	KEYCODE_L,
	KEYCODE_M,
	KEYCODE_N,
	KEYCODE_O,
	KEYCODE_P,
	KEYCODE_Q,
	KEYCODE_R,
	KEYCODE_S,
	KEYCODE_T,
	KEYCODE_U,
	KEYCODE_V,
	KEYCODE_W,
	KEYCODE_X,
	KEYCODE_Y,
	KEYCODE_Z,

	KEYCODE_0,
	KEYCODE_1,
	KEYCODE_2,
	KEYCODE_3,
	KEYCODE_4,
	KEYCODE_5,
	KEYCODE_6,
	KEYCODE_7,
	KEYCODE_8,
	KEYCODE_9,

	KEYCODE_F1,
	KEYCODE_F2,
	KEYCODE_F3,
	KEYCODE_F4,
	KEYCODE_F5,
	KEYCODE_F6,
	KEYCODE_F7,
	KEYCODE_F8,
	KEYCODE_F9,
	KEYCODE_F10,
	KEYCODE_F11,
	KEYCODE_F12,
	KEYCODE_F13,
	KEYCODE_F14,
	KEYCODE_F15,
	KEYCODE_F16,
	KEYCODE_F17,
	KEYCODE_F18,
	KEYCODE_F19,
	KEYCODE_F20,
	KEYCODE_F21,
	KEYCODE_F22,
	KEYCODE_F23,
	KEYCODE_F24,

	KEYCODE_UP,
	KEYCODE_DOWN,
	KEYCODE_LEFT,
	KEYCODE_RIGHT,
	KEYCODE_HOME,
	KEYCODE_END,
	KEYCODE_PAGE_UP,
	KEYCODE_PAGE_DOWN,
	KEYCODE_INSERT,
	KEYCODE_DELETE,

	KEYCODE_ESCAPE,
	KEYCODE_TAB,
	KEYCODE_CAPS_LOCK,
	KEYCODE_SPACE,
	KEYCODE_ENTER,
	KEYCODE_BACKSPACE,

	KEYCODE_LEFT_SHIFT,
	KEYCODE_RIGHT_SHIFT,
	KEYCODE_LEFT_CTRL,
	KEYCODE_RIGHT_CTRL,
	KEYCODE_LEFT_ALT,
	KEYCODE_RIGHT_ALT,
	KEYCODE_LEFT_SUPER,
	KEYCODE_RIGHT_SUPER,
	KEYCODE_MENU,

	KEYCODE_NUMPAD_0,
	KEYCODE_NUMPAD_1,
	KEYCODE_NUMPAD_2,
	KEYCODE_NUMPAD_3,
	KEYCODE_NUMPAD_4,
	KEYCODE_NUMPAD_5,
	KEYCODE_NUMPAD_6,
	KEYCODE_NUMPAD_7,
	KEYCODE_NUMPAD_8,
	KEYCODE_NUMPAD_9,
	KEYCODE_NUMPAD_MULTIPLY,
	KEYCODE_NUMPAD_ADD,
	KEYCODE_NUMPAD_SEPARATOR,
	KEYCODE_NUMPAD_SUBTRACT,
	KEYCODE_NUMPAD_DECIMAL,
	KEYCODE_NUMPAD_DIVIDE,
	KEYCODE_NUMPAD_ENTER,

	KEYCODE_SCROLL_LOCK,
	KEYCODE_NUM_LOCK,
	KEYCODE_PAUSE,

	KEYCODE_GRAVE,
	KEYCODE_MINUS,
	KEYCODE_EQUALS,
	KEYCODE_LEFT_BRACKET,
	KEYCODE_RIGHT_BRACKET,
	KEYCODE_BACKSLASH,
	KEYCODE_SEMICOLON,
	KEYCODE_APOSTROPHE,
	KEYCODE_COMMA,
	KEYCODE_PERIOD,
	KEYCODE_SLASH,

	KEYCODE_VOLUME_MUTE,
	KEYCODE_VOLUME_DOWN,
	KEYCODE_VOLUME_UP,
	KEYCODE_MEDIA_NEXT,
	KEYCODE_MEDIA_PREV,
	KEYCODE_MEDIA_STOP,
	KEYCODE_MEDIA_PLAY_PAUSE,

	KEYCODE_BROWSER_BACK,
	KEYCODE_BROWSER_FORWARD,
	KEYCODE_BROWSER_REFRESH,
	KEYCODE_BROWSER_STOP,
	KEYCODE_BROWSER_SEARCH,
	KEYCODE_BROWSER_FAVORITES,
	KEYCODE_BROWSER_HOME,

	KEYCODE_LAUNCH_MAIL,
	KEYCODE_LAUNCH_MEDIA_SELECT,
	KEYCODE_LAUNCH_APP1,
	KEYCODE_LAUNCH_APP2,

	KEYCODE_MOUSE_LEFT,
	KEYCODE_MOUSE_RIGHT,
	KEYCODE_MOUSE_MIDDLE,
	KEYCODE_MOUSE_X1,
	KEYCODE_MOUSE_X2,

	KEYCODE_COUNT
};

export enum EModifier : unsigned char {
	MODIFIER_NONE = 0,
	MODIFIER_SHIFT = 1 << 0,
	MODIFIER_CTRL = 1 << 1,
	MODIFIER_ALT = 1 << 2,
	MODIFIER_SUPER = 1 << 3,
	MODIFIER_CAPS_LOCK = 1 << 4,
	MODIFIER_NUM_LOCK = 1 << 5,
	MODIFIER_INSERT = 1 << 6,
	MODIFIER_SCREEN_READER = 1 << 7
};

export struct SHotkeyInfo final {
	EKeycode keycode;
	unsigned char modifiers;

	explicit constexpr SHotkeyInfo(EKeycode keycode = KEYCODE_NONE, unsigned char modifiers = MODIFIER_NONE)
		: keycode(keycode), modifiers(modifiers) {}

	static constexpr auto GetAny() -> SHotkeyInfo { return SHotkeyInfo(KEYCODE_ANY, MODIFIER_NONE); }

	[[nodiscard]] constexpr auto Pack() const noexcept -> uint32_t {
		return (static_cast<uint32_t>(keycode) << 8) | modifiers;
	}

	constexpr auto operator==(const SHotkeyInfo& info) const -> bool { return Pack() == info.Pack(); }
};

// Allow operator+ to generate key bindings.
export [[nodiscard]] constexpr auto operator+(EModifier a, EModifier b) -> unsigned char {
	return static_cast<unsigned char>(a) | static_cast<unsigned char>(b);
}

export [[nodiscard]] constexpr auto operator+(unsigned char a, EModifier b) -> unsigned char {
	return a | static_cast<unsigned char>(b);
}

export [[nodiscard]] constexpr auto operator+(unsigned char mods, EKeycode key) -> SHotkeyInfo {
	return SHotkeyInfo(key, mods);
}

export [[nodiscard]] constexpr auto operator+(EKeycode key, unsigned char mods) -> SHotkeyInfo {
	return SHotkeyInfo(key, mods);
}

export [[nodiscard]] constexpr inline auto GetKeycodeName(EKeycode keycode) -> std::string_view {
	switch (keycode) {
	case KEYCODE_A:
		return "A";
	case KEYCODE_B:
		return "B";
	case KEYCODE_C:
		return "C";
	case KEYCODE_D:
		return "D";
	case KEYCODE_E:
		return "E";
	case KEYCODE_F:
		return "F";
	case KEYCODE_G:
		return "G";
	case KEYCODE_H:
		return "H";
	case KEYCODE_I:
		return "I";
	case KEYCODE_J:
		return "J";
	case KEYCODE_K:
		return "K";
	case KEYCODE_L:
		return "L";
	case KEYCODE_M:
		return "M";
	case KEYCODE_N:
		return "N";
	case KEYCODE_O:
		return "O";
	case KEYCODE_P:
		return "P";
	case KEYCODE_Q:
		return "Q";
	case KEYCODE_R:
		return "R";
	case KEYCODE_S:
		return "S";
	case KEYCODE_T:
		return "T";
	case KEYCODE_U:
		return "U";
	case KEYCODE_V:
		return "V";
	case KEYCODE_W:
		return "W";
	case KEYCODE_X:
		return "X";
	case KEYCODE_Y:
		return "Y";
	case KEYCODE_Z:
		return "Z";

	case KEYCODE_0:
		return "0";
	case KEYCODE_1:
		return "1";
	case KEYCODE_2:
		return "2";
	case KEYCODE_3:
		return "3";
	case KEYCODE_4:
		return "4";
	case KEYCODE_5:
		return "5";
	case KEYCODE_6:
		return "6";
	case KEYCODE_7:
		return "7";
	case KEYCODE_8:
		return "8";
	case KEYCODE_9:
		return "9";

	case KEYCODE_F1:
		return "F1";
	case KEYCODE_F2:
		return "F2";
	case KEYCODE_F3:
		return "F3";
	case KEYCODE_F4:
		return "F4";
	case KEYCODE_F5:
		return "F5";
	case KEYCODE_F6:
		return "F6";
	case KEYCODE_F7:
		return "F7";
	case KEYCODE_F8:
		return "F8";
	case KEYCODE_F9:
		return "F9";
	case KEYCODE_F10:
		return "F10";
	case KEYCODE_F11:
		return "F11";
	case KEYCODE_F12:
		return "F12";
	case KEYCODE_F13:
		return "F13";
	case KEYCODE_F14:
		return "F14";
	case KEYCODE_F15:
		return "F15";
	case KEYCODE_F16:
		return "F16";
	case KEYCODE_F17:
		return "F17";
	case KEYCODE_F18:
		return "F18";
	case KEYCODE_F19:
		return "F19";
	case KEYCODE_F20:
		return "F20";
	case KEYCODE_F21:
		return "F21";
	case KEYCODE_F22:
		return "F22";
	case KEYCODE_F23:
		return "F23";
	case KEYCODE_F24:
		return "F24";

	case KEYCODE_UP:
		return "Up";
	case KEYCODE_DOWN:
		return "Down";
	case KEYCODE_LEFT:
		return "Left";
	case KEYCODE_RIGHT:
		return "Right";
	case KEYCODE_HOME:
		return "Home";
	case KEYCODE_END:
		return "End";
	case KEYCODE_PAGE_UP:
		return "Page Up";
	case KEYCODE_PAGE_DOWN:
		return "Page Down";
	case KEYCODE_INSERT:
		return "Insert";
	case KEYCODE_DELETE:
		return "Delete";

	case KEYCODE_ESCAPE:
		return "Escape";
	case KEYCODE_TAB:
		return "Tab";
	case KEYCODE_CAPS_LOCK:
		return "Caps Lock";
	case KEYCODE_SPACE:
		return "Space";
	case KEYCODE_ENTER:
		return "Enter";
	case KEYCODE_BACKSPACE:
		return "Backspace";

	case KEYCODE_LEFT_SHIFT:
		return "Left Shift";
	case KEYCODE_RIGHT_SHIFT:
		return "Right Shift";
	case KEYCODE_LEFT_CTRL:
		return "Left Ctrl";
	case KEYCODE_RIGHT_CTRL:
		return "Right Ctrl";
	case KEYCODE_LEFT_ALT:
		return "Left Alt";
	case KEYCODE_RIGHT_ALT:
		return "Right Alt";
	case KEYCODE_LEFT_SUPER:
		return "Left Super";
	case KEYCODE_RIGHT_SUPER:
		return "Right Super";
	case KEYCODE_MENU:
		return "Menu";

	case KEYCODE_NUMPAD_0:
		return "Numpad 0";
	case KEYCODE_NUMPAD_1:
		return "Numpad 1";
	case KEYCODE_NUMPAD_2:
		return "Numpad 2";
	case KEYCODE_NUMPAD_3:
		return "Numpad 3";
	case KEYCODE_NUMPAD_4:
		return "Numpad 4";
	case KEYCODE_NUMPAD_5:
		return "Numpad 5";
	case KEYCODE_NUMPAD_6:
		return "Numpad 6";
	case KEYCODE_NUMPAD_7:
		return "Numpad 7";
	case KEYCODE_NUMPAD_8:
		return "Numpad 8";
	case KEYCODE_NUMPAD_9:
		return "Numpad 9";
	case KEYCODE_NUMPAD_MULTIPLY:
		return "Numpad *";
	case KEYCODE_NUMPAD_ADD:
		return "Numpad +";
	case KEYCODE_NUMPAD_SEPARATOR:
		return "Numpad Separator";
	case KEYCODE_NUMPAD_SUBTRACT:
		return "Numpad -";
	case KEYCODE_NUMPAD_DECIMAL:
		return "Numpad Decimal";
	case KEYCODE_NUMPAD_DIVIDE:
		return "Numpad /";
	case KEYCODE_NUMPAD_ENTER:
		return "Numpad Enter";

	case KEYCODE_SCROLL_LOCK:
		return "Scroll Lock";
	case KEYCODE_NUM_LOCK:
		return "Num Lock";
	case KEYCODE_PAUSE:
		return "Pause";

	case KEYCODE_GRAVE:
		return "`";
	case KEYCODE_MINUS:
		return "-";
	case KEYCODE_EQUALS:
		return "=";
	case KEYCODE_LEFT_BRACKET:
		return "[";
	case KEYCODE_RIGHT_BRACKET:
		return "]";
	case KEYCODE_BACKSLASH:
		return "\\";
	case KEYCODE_SEMICOLON:
		return ";";
	case KEYCODE_APOSTROPHE:
		return "'";
	case KEYCODE_COMMA:
		return ",";
	case KEYCODE_PERIOD:
		return ".";
	case KEYCODE_SLASH:
		return "/";

	case KEYCODE_VOLUME_MUTE:
		return "Volume Mute";
	case KEYCODE_VOLUME_DOWN:
		return "Volume Down";
	case KEYCODE_VOLUME_UP:
		return "Volume Up";
	case KEYCODE_MEDIA_NEXT:
		return "Media Next";
	case KEYCODE_MEDIA_PREV:
		return "Media Previous";
	case KEYCODE_MEDIA_STOP:
		return "Media Stop";
	case KEYCODE_MEDIA_PLAY_PAUSE:
		return "Media Play/Pause";

	case KEYCODE_BROWSER_BACK:
		return "Browser Back";
	case KEYCODE_BROWSER_FORWARD:
		return "Browser Forward";
	case KEYCODE_BROWSER_REFRESH:
		return "Browser Refresh";
	case KEYCODE_BROWSER_STOP:
		return "Browser Stop";
	case KEYCODE_BROWSER_SEARCH:
		return "Browser Search";
	case KEYCODE_BROWSER_FAVORITES:
		return "Browser Favorites";
	case KEYCODE_BROWSER_HOME:
		return "Browser Home";

	case KEYCODE_LAUNCH_MAIL:
		return "Launch Mail";
	case KEYCODE_LAUNCH_MEDIA_SELECT:
		return "Launch Media Select";
	case KEYCODE_LAUNCH_APP1:
		return "Launch App1";
	case KEYCODE_LAUNCH_APP2:
		return "Launch App2";

	case KEYCODE_MOUSE_LEFT:
		return "Mouse Left";
	case KEYCODE_MOUSE_RIGHT:
		return "Mouse Right";
	case KEYCODE_MOUSE_MIDDLE:
		return "Mouse Middle";
	case KEYCODE_MOUSE_X1:
		return "Mouse X1";
	case KEYCODE_MOUSE_X2:
		return "Mouse X2";

	case KEYCODE_NONE:
		return "None";
	case KEYCODE_ANY:
		return "Any";

	default:
		return "None";
	}
}
