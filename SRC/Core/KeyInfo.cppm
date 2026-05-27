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

export [[nodiscard]] constexpr auto GetModifierFromKeycode(EKeycode keycode) -> EModifier {
	switch (keycode) {
	case KEYCODE_LEFT_SHIFT:
	case KEYCODE_RIGHT_SHIFT:
		return MODIFIER_SHIFT;
	case KEYCODE_LEFT_CTRL:
	case KEYCODE_RIGHT_CTRL:
		return MODIFIER_CTRL;
	case KEYCODE_LEFT_ALT:
	case KEYCODE_RIGHT_ALT:
		return MODIFIER_ALT;
	case KEYCODE_LEFT_SUPER:
	case KEYCODE_RIGHT_SUPER:
		return MODIFIER_SUPER;
	case KEYCODE_CAPS_LOCK:
		return MODIFIER_CAPS_LOCK;
	case KEYCODE_NUM_LOCK:
		return MODIFIER_NUM_LOCK;
	case KEYCODE_INSERT:
		return MODIFIER_INSERT;
	default:
		return MODIFIER_NONE;
	}
}
