// An interface for abstracting event types. Categories are implemented here.
#pragma once
#include <memory>
#include <optional>
#include <string_view>
#include <string>
#include <variant>
#include <memory_resource>
#include <vector>

class IObject;

/*
The object event category.
Events such as changes to an object's focus, state, name, description, value, etc., always dispatch a CObjectEvent.
*/
class CObjectEvent final {
public:

	enum EObjectEventType : unsigned char {
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
	} type;

	std::shared_ptr<IObject> object;
};

/*
The keyboard event category.
Key pressed and released events. always dispatch a CKeyboardEvent.
*/
class CKeyboardEvent final {
public:

	enum eKeyboardEventType : unsigned char {
		NONE = 0,
		KEY_PRESSED,
		KEY_RELEASED
	} type;

	enum EKeycode : unsigned short {
		KEYCODE_A = 'A',
		KEYCODE_B = 'B',
		KEYCODE_C = 'C',
		KEYCODE_D = 'D',
		KEYCODE_E = 'E',
		KEYCODE_F = 'F',
		KEYCODE_G = 'G',
		KEYCODE_H = 'H',
		KEYCODE_I = 'I',
		KEYCODE_J = 'J',
		KEYCODE_K = 'K',
		KEYCODE_L = 'L',
		KEYCODE_M = 'M',
		KEYCODE_N = 'N',
		KEYCODE_O = 'O',
		KEYCODE_P = 'P',
		KEYCODE_Q = 'Q',
		KEYCODE_R = 'R',
		KEYCODE_S = 'S',
		KEYCODE_T = 'T',
		KEYCODE_U = 'U',
		KEYCODE_V = 'V',
		KEYCODE_W = 'W',
		KEYCODE_X = 'X',
		KEYCODE_Y = 'Y',
		KEYCODE_Z = 'Z',

		KEYCODE_0 = '0',
		KEYCODE_1 = '1',
		KEYCODE_2 = '2',
		KEYCODE_3 = '3',
		KEYCODE_4 = '4',
		KEYCODE_5 = '5',
		KEYCODE_6 = '6',
		KEYCODE_7 = '7',
		KEYCODE_8 = '8',
		KEYCODE_9 = '9',

		KEYCODE_F1 = 0x70,
		KEYCODE_F2 = 113,
		KEYCODE_F3 = 114,
		KEYCODE_F4 = 115,
		KEYCODE_F5 = 116,
		KEYCODE_F6 = 117,
		KEYCODE_F7 = 118,
		KEYCODE_F8 = 119,
		KEYCODE_F9 = 120,
		KEYCODE_F10 = 121,
		KEYCODE_F11 = 122,
		KEYCODE_F12 = 123,
		KEYCODE_F13 = 124,
		KEYCODE_F14 = 125,
		KEYCODE_F15 = 126,
		KEYCODE_F16 = 127,
		KEYCODE_F17 = 128,
		KEYCODE_F18 = 129,
		KEYCODE_F19 = 130,
		KEYCODE_F20 = 131,
		KEYCODE_F21 = 132,
		KEYCODE_F22 = 133,
		KEYCODE_F23 = 134,
		KEYCODE_F24 = 135,

		KEYCODE_UP = 0x26,
		KEYCODE_DOWN = 0x28,
		KEYCODE_LEFT = 0x25,
		KEYCODE_RIGHT = 0x27,
		KEYCODE_HOME = 0x24,
		KEYCODE_END = 0x23,
		KEYCODE_PAGE_UP = 0x21,
		KEYCODE_PAGE_DOWN = 0x22,
		KEYCODE_INSERT = 0x2D,
		KEYCODE_DELETE = 0x2E,

		KEYCODE_ESCAPE = 0x1B,
		KEYCODE_TAB = 0x09,
		KEYCODE_CAPS_LOCK = 0x14,
		KEYCODE_SHIFT = 0x10,
		KEYCODE_CTRL = 0x11,
		KEYCODE_ALT = 0x12,
		KEYCODE_SPACE = 0x20,
		KEYCODE_ENTER = 0x0D,
		KEYCODE_BACKSPACE = 0x08,

		KEYCODE_LEFT_SHIFT = 0xA0,
		KEYCODE_RIGHT_SHIFT = 0xA1,
		KEYCODE_LEFT_CTRL = 0xA2,
		KEYCODE_RIGHT_CTRL = 0xA3,
		KEYCODE_LEFT_ALT = 0xA4,
		KEYCODE_RIGHT_ALT = 0xA5,
		KEYCODE_LEFT_SUPER = 0x5B,
		KEYCODE_RIGHT_SUPER = 0x5C,
		KEYCODE_MENU = 0x5D,

		KEYCODE_NUMPAD_0 = 0x60,
		KEYCODE_NUMPAD_1 = 97,
		KEYCODE_NUMPAD_2 = 98,
		KEYCODE_NUMPAD_3 = 99,
		KEYCODE_NUMPAD_4 = 100,
		KEYCODE_NUMPAD_5 = 101,
		KEYCODE_NUMPAD_6 = 102,
		KEYCODE_NUMPAD_7 = 103,
		KEYCODE_NUMPAD_8 = 104,
		KEYCODE_NUMPAD_9 = 105,
		KEYCODE_NUMPAD_MULTIPLY = 106,
		KEYCODE_NUMPAD_ADD = 107,
		KEYCODE_NUMPAD_SEPARATOR = 108,
		KEYCODE_NUMPAD_SUBTRACT = 109,
		KEYCODE_NUMPAD_DECIMAL = 110,
		KEYCODE_NUMPAD_DIVIDE = 111,
		KEYCODE_NUMPAD_ENTER = 0xE0,

		KEYCODE_SCROLL_LOCK = 0x91,
		KEYCODE_NUM_LOCK = 0x90,
		KEYCODE_PAUSE = 0x13,

		KEYCODE_GRAVE = 0xC0,
		KEYCODE_MINUS = 0xBD,
		KEYCODE_EQUALS = 0xBB,
		KEYCODE_LEFT_BRACKET = 0xDB,
		KEYCODE_RIGHT_BRACKET = 0xDD,
		KEYCODE_BACKSLASH = 0xDC,
		KEYCODE_SEMICOLON = 0xBA,
		KEYCODE_APOSTROPHE = 0xDE,
		KEYCODE_COMMA = 0xBC,
		KEYCODE_PERIOD = 0xBE,
		KEYCODE_SLASH = 0xBF,

		KEYCODE_VOLUME_MUTE = 0xAD,
		KEYCODE_VOLUME_DOWN = 0xAE,
		KEYCODE_VOLUME_UP = 0xAF,
		KEYCODE_MEDIA_NEXT = 0xB0,
		KEYCODE_MEDIA_PREV = 0xB1,
		KEYCODE_MEDIA_STOP = 0xB2,
		KEYCODE_MEDIA_PLAY_PAUSE = 0xB3,

		KEYCODE_BROWSER_BACK = 0xA6,
		KEYCODE_BROWSER_FORWARD = 0xA7,
		KEYCODE_BROWSER_REFRESH = 0xA8,
		KEYCODE_BROWSER_STOP = 0xA9,
		KEYCODE_BROWSER_SEARCH = 0xAA,
		KEYCODE_BROWSER_FAVORITES = 0xAB,
		KEYCODE_BROWSER_HOME = 0xAC,

		KEYCODE_LAUNCH_MAIL = 0xB4,
		KEYCODE_LAUNCH_MEDIA_SELECT = 0xB5,
		KEYCODE_LAUNCH_APP1 = 0xB6,
		KEYCODE_LAUNCH_APP2 = 0xB7,

		KEYCODE_MOUSE_LEFT = 0x01,
		KEYCODE_MOUSE_RIGHT = 0x02,
		KEYCODE_MOUSE_MIDDLE = 0x04,
		KEYCODE_MOUSE_X1 = 0x05,
		KEYCODE_MOUSE_X2 = 0x06,

		KEYCODE_NONE = 0x00,
		KEYCODE_ANY = 0xFFFF
	};

	enum EModifier : unsigned char {
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

	struct SHotkeyInfo final {
		EKeycode keycode;
		unsigned char modifiers;

		explicit constexpr SHotkeyInfo(const EKeycode& keycode = KEYCODE_NONE, const unsigned char& modifiers = MODIFIER_NONE) : keycode(keycode), modifiers(modifiers) {}

		static constexpr auto GetAny() -> SHotkeyInfo {
			return SHotkeyInfo(KEYCODE_ANY, MODIFIER_NONE);
		}

		[[nodiscard]] constexpr auto Pack() const noexcept -> uint32_t {
			return (static_cast<uint32_t>(keycode) << 8) | modifiers;
		}

		constexpr auto operator==(const SHotkeyInfo& info) const -> bool {
			return Pack() == info.Pack();
		}
	} hotkey;

	[[nodiscard]] constexpr static inline auto GetKeycodeName(const EKeycode& keycode) -> std::string_view {
		switch (keycode) {
			case KEYCODE_A: return "A";
			case KEYCODE_B: return "B";
			case KEYCODE_C: return "C";
			case KEYCODE_D: return "D";
			case KEYCODE_E: return "E";
			case KEYCODE_F: return "F";
			case KEYCODE_G: return "G";
			case KEYCODE_H: return "H";
			case KEYCODE_I: return "I";
			case KEYCODE_J: return "J";
			case KEYCODE_K: return "K";
			case KEYCODE_L: return "L";
			case KEYCODE_M: return "M";
			case KEYCODE_N: return "N";
			case KEYCODE_O: return "O";
			case KEYCODE_P: return "P";
			case KEYCODE_Q: return "Q";
			case KEYCODE_R: return "R";
			case KEYCODE_S: return "S";
			case KEYCODE_T: return "T";
			case KEYCODE_U: return "U";
			case KEYCODE_V: return "V";
			case KEYCODE_W: return "W";
			case KEYCODE_X: return "X";
			case KEYCODE_Y: return "Y";
			case KEYCODE_Z: return "Z";

			case KEYCODE_0: return "0";
			case KEYCODE_1: return "1";
			case KEYCODE_2: return "2";
			case KEYCODE_3: return "3";
			case KEYCODE_4: return "4";
			case KEYCODE_5: return "5";
			case KEYCODE_6: return "6";
			case KEYCODE_7: return "7";
			case KEYCODE_8: return "8";
			case KEYCODE_9: return "9";

			case KEYCODE_F1: return "F1";
			case KEYCODE_F2: return "F2";
			case KEYCODE_F3: return "F3";
			case KEYCODE_F4: return "F4";
			case KEYCODE_F5: return "F5";
			case KEYCODE_F6: return "F6";
			case KEYCODE_F7: return "F7";
			case KEYCODE_F8: return "F8";
			case KEYCODE_F9: return "F9";
			case KEYCODE_F10: return "F10";
			case KEYCODE_F11: return "F11";
			case KEYCODE_F12: return "F12";
			case KEYCODE_F13: return "F13";
			case KEYCODE_F14: return "F14";
			case KEYCODE_F15: return "F15";
			case KEYCODE_F16: return "F16";
			case KEYCODE_F17: return "F17";
			case KEYCODE_F18: return "F18";
			case KEYCODE_F19: return "F19";
			case KEYCODE_F20: return "F20";
			case KEYCODE_F21: return "F21";
			case KEYCODE_F22: return "F22";
			case KEYCODE_F23: return "F23";
			case KEYCODE_F24: return "F24";

			case KEYCODE_UP: return "Up";
			case KEYCODE_DOWN: return "Down";
			case KEYCODE_LEFT: return "Left";
			case KEYCODE_RIGHT: return "Right";
			case KEYCODE_HOME: return "Home";
			case KEYCODE_END: return "End";
			case KEYCODE_PAGE_UP: return "Page Up";
			case KEYCODE_PAGE_DOWN: return "Page Down";
			case KEYCODE_INSERT: return "Insert";
			case KEYCODE_DELETE: return "Delete";

			case KEYCODE_ESCAPE: return "Escape";
			case KEYCODE_TAB: return "Tab";
			case KEYCODE_CAPS_LOCK: return "Caps Lock";
			case KEYCODE_SHIFT: return "Shift";
			case KEYCODE_CTRL: return "Ctrl";
			case KEYCODE_ALT: return "Alt";
			case KEYCODE_SPACE: return "Space";
			case KEYCODE_ENTER: return "Enter";
			case KEYCODE_BACKSPACE: return "Backspace";

			case KEYCODE_LEFT_SHIFT: return "Left Shift";
			case KEYCODE_RIGHT_SHIFT: return "Right Shift";
			case KEYCODE_LEFT_CTRL: return "Left Ctrl";
			case KEYCODE_RIGHT_CTRL: return "Right Ctrl";
			case KEYCODE_LEFT_ALT: return "Left Alt";
			case KEYCODE_RIGHT_ALT: return "Right Alt";
			case KEYCODE_LEFT_SUPER: return "Left Super";
			case KEYCODE_RIGHT_SUPER: return "Right Super";
			case KEYCODE_MENU: return "Menu";

			case KEYCODE_NUMPAD_0: return "Numpad 0";
			case KEYCODE_NUMPAD_1: return "Numpad 1";
			case KEYCODE_NUMPAD_2: return "Numpad 2";
			case KEYCODE_NUMPAD_3: return "Numpad 3";
			case KEYCODE_NUMPAD_4: return "Numpad 4";
			case KEYCODE_NUMPAD_5: return "Numpad 5";
			case KEYCODE_NUMPAD_6: return "Numpad 6";
			case KEYCODE_NUMPAD_7: return "Numpad 7";
			case KEYCODE_NUMPAD_8: return "Numpad 8";
			case KEYCODE_NUMPAD_9: return "Numpad 9";
			case KEYCODE_NUMPAD_MULTIPLY: return "Numpad *";
			case KEYCODE_NUMPAD_ADD: return "Numpad +";
			case KEYCODE_NUMPAD_SEPARATOR: return "Numpad Separator";
			case KEYCODE_NUMPAD_SUBTRACT: return "Numpad -";
			case KEYCODE_NUMPAD_DECIMAL: return "Numpad Decimal";
			case KEYCODE_NUMPAD_DIVIDE: return "Numpad /";
			case KEYCODE_NUMPAD_ENTER: return "Numpad Enter";

			case KEYCODE_SCROLL_LOCK: return "Scroll Lock";
			case KEYCODE_NUM_LOCK: return "Num Lock";
			case KEYCODE_PAUSE: return "Pause";

			case KEYCODE_GRAVE: return "`";
			case KEYCODE_MINUS: return "-";
			case KEYCODE_EQUALS: return "=";
			case KEYCODE_LEFT_BRACKET: return "[";
			case KEYCODE_RIGHT_BRACKET: return "]";
			case KEYCODE_BACKSLASH: return "\\";
			case KEYCODE_SEMICOLON: return ";";
			case KEYCODE_APOSTROPHE: return "'";
			case KEYCODE_COMMA: return ",";
			case KEYCODE_PERIOD: return ".";
			case KEYCODE_SLASH: return "/";

			case KEYCODE_VOLUME_MUTE: return "Volume Mute";
			case KEYCODE_VOLUME_DOWN: return "Volume Down";
			case KEYCODE_VOLUME_UP: return "Volume Up";
			case KEYCODE_MEDIA_NEXT: return "Media Next";
			case KEYCODE_MEDIA_PREV: return "Media Previous";
			case KEYCODE_MEDIA_STOP: return "Media Stop";
			case KEYCODE_MEDIA_PLAY_PAUSE: return "Media Play/Pause";

			case KEYCODE_BROWSER_BACK: return "Browser Back";
			case KEYCODE_BROWSER_FORWARD: return "Browser Forward";
			case KEYCODE_BROWSER_REFRESH: return "Browser Refresh";
			case KEYCODE_BROWSER_STOP: return "Browser Stop";
			case KEYCODE_BROWSER_SEARCH: return "Browser Search";
			case KEYCODE_BROWSER_FAVORITES: return "Browser Favorites";
			case KEYCODE_BROWSER_HOME: return "Browser Home";

			case KEYCODE_LAUNCH_MAIL: return "Launch Mail";
			case KEYCODE_LAUNCH_MEDIA_SELECT: return "Launch Media Select";
			case KEYCODE_LAUNCH_APP1: return "Launch App1";
			case KEYCODE_LAUNCH_APP2: return "Launch App2";

			case KEYCODE_MOUSE_LEFT: return "Mouse Left";
			case KEYCODE_MOUSE_RIGHT: return "Mouse Right";
			case KEYCODE_MOUSE_MIDDLE: return "Mouse Middle";
			case KEYCODE_MOUSE_X1: return "Mouse X1";
			case KEYCODE_MOUSE_X2: return "Mouse X2";

			case KEYCODE_NONE: return "None";
			case KEYCODE_ANY: return "Any";

			default: return "None";
		}
	}

	[[nodiscard]] static inline auto GetModifierNames(const unsigned char& modifiers) -> std::string {
		if (modifiers == MODIFIER_NONE) {
			return "None";
		}

		std::vector<std::string> modifier_names;

		if (modifiers & MODIFIER_SHIFT) modifier_names.emplace_back("Shift");
		if (modifiers & MODIFIER_CTRL) modifier_names.emplace_back("Ctrl");
		if (modifiers & MODIFIER_ALT) modifier_names.emplace_back("Alt");
		if (modifiers & MODIFIER_SUPER) modifier_names.emplace_back("Super");
		if (modifiers & MODIFIER_CAPS_LOCK) modifier_names.emplace_back("Caps Lock");
		if (modifiers & MODIFIER_NUM_LOCK) modifier_names.emplace_back("Num Lock");

		if (modifier_names.empty()) {
			return "None";
		}

		std::string result;
		for (size_t i = 0; i < modifier_names.size(); ++i) {
			if (i > 0) {
				result += "+";
			}
			result += modifier_names[i];
		}
		return result;
	}

	[[nodiscard]] static inline auto KeyComboToString(const EKeycode& keycode, const unsigned char& modifiers) -> std::string {
		std::string modifier_names = GetModifierNames(modifiers);
		std::string_view keycode_name = GetKeycodeName(keycode);

		if (modifier_names == "None") {
			return std::string(keycode_name);
		}
		else {
			return modifier_names + "+" + std::string(keycode_name);
		}
	}
};

// Allow operator+ to generate key bindings.
[[nodiscard]] constexpr auto operator+(CKeyboardEvent::EModifier a, CKeyboardEvent::EModifier b) -> unsigned char {
	return static_cast<unsigned char>(a) | static_cast<unsigned char>(b);
}

[[nodiscard]] constexpr auto operator+(unsigned char a, CKeyboardEvent::EModifier b) -> unsigned char {
	return a | static_cast<unsigned char>(b);
}

[[nodiscard]] constexpr auto operator+(unsigned char mods, CKeyboardEvent::EKeycode key) -> CKeyboardEvent::SHotkeyInfo {
	return CKeyboardEvent::SHotkeyInfo(key, mods);
}

[[nodiscard]] constexpr auto operator+(CKeyboardEvent::EKeycode key, unsigned char mods) -> CKeyboardEvent::SHotkeyInfo {
	return CKeyboardEvent::SHotkeyInfo(key, mods);
}

using SHotkeyInfo = CKeyboardEvent::SHotkeyInfo;

namespace std {
	template<>
	struct hash<CKeyboardEvent::SHotkeyInfo> {
		auto operator()(const CKeyboardEvent::SHotkeyInfo& k) const noexcept -> std::size_t {
			return std::hash<uint32_t>{}(k.Pack());
		}
	};
}

using EventVariant = std::variant<std::monostate, CObjectEvent, CKeyboardEvent>;

class CEvent final {
	EventVariant m_variant;
	bool m_now{false}; // A very specific flag. I'll describe it in the handlers.
public:

	using allocator_type = std::pmr::polymorphic_allocator<>;

	enum EEventType : unsigned char {
		NONE = 0,
		OBJECT,
		KEYBOARD
	};

private:
	EEventType m_type{NONE};
public:

	~CEvent() = default;
	CEvent(CObjectEvent&& object_event, bool now = false, allocator_type alloc = {})
		: m_variant(std::move(object_event)), m_type(OBJECT), m_now(now) {}

	CEvent(CKeyboardEvent&& keyboard_event, bool now = false, allocator_type alloc = {})
		: m_variant(std::move(keyboard_event)), m_type(KEYBOARD), m_now(now) {}

	CEvent(CEvent&& other, allocator_type alloc)
		: m_variant(std::move(other.m_variant)), 
			m_now(other.m_now), 
							m_type(other.m_type) {}

	CEvent(CEvent&&) = default;
	CEvent& operator=(CEvent&&) = default;

	[[nodiscard]] auto GetType() const -> EEventType { 
		return m_type;
	}

	[[nodiscard]] auto GetNow() const -> bool { 
		return m_now;
	}

	template<typename T>
	auto GetAs(this auto&& self) -> std::optional<std::conditional_t<std::is_const_v<std::remove_reference_t<decltype(self)>>, const T, T>> {
		if (auto* ptr = std::get_if<T>(&self.m_variant)) {
			return *ptr;
		}
		return std::nullopt;
	}

	template<typename Visitor>
	auto Visit(Visitor&& visitor) -> decltype(auto) {
		return std::visit(std::forward<Visitor>(visitor), m_variant);
	}

	template<typename Visitor>
	auto Visit(Visitor&& visitor) const -> decltype(auto) {
		return std::visit(std::forward<Visitor>(visitor), m_variant);
	}
};

