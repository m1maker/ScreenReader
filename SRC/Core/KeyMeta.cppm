module;
#include <array>
#include <bitset>
#include <string_view>
#include <utility>
export module Core.KeyMeta;
import Core.KeyInfo;

export enum class EKeyGroup : unsigned char {
	UNKNOWN = 0,
	UNPREDICTABLE,
	TYPABLE,
	CHARACTER,
	DIGIT,
	ALPHABET,
	PUNCTUATION,
	FUNCTIONAL,
	NAVIGATIONAL,
	EDITING,
	NUMPAD,
	MODIFIER,
	MEDIA,
	APPLICATION,
	COUNT
};

using KeyGroupMask = std::bitset<std::to_underlying(EKeyGroup::COUNT)>;

struct SKeyMeta final {
	std::string_view name;
	KeyGroupMask group_flags;
};

[[nodiscard]] static consteval auto InitializeMeta(EKeycode keycode) -> SKeyMeta {
	SKeyMeta meta;

	switch (keycode) {
	case EKeycode::KEYCODE_A:
		meta.name = "A";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_B:
		meta.name = "B";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_C:
		meta.name = "C";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_D:
		meta.name = "D";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_E:
		meta.name = "E";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_F:
		meta.name = "F";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_G:
		meta.name = "G";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_H:
		meta.name = "H";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_I:
		meta.name = "I";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_J:
		meta.name = "J";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_K:
		meta.name = "K";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_L:
		meta.name = "L";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_M:
		meta.name = "M";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_N:
		meta.name = "N";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_O:
		meta.name = "O";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_P:
		meta.name = "P";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_Q:
		meta.name = "Q";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_R:
		meta.name = "R";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_S:
		meta.name = "S";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_T:
		meta.name = "T";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_U:
		meta.name = "U";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_V:
		meta.name = "V";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_W:
		meta.name = "W";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_X:
		meta.name = "X";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_Y:
		meta.name = "Y";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;
	case EKeycode::KEYCODE_Z:
		meta.name = "Z";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::ALPHABET));
		break;

	case EKeycode::KEYCODE_0:
		meta.name = "0";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::DIGIT));
		break;
	case EKeycode::KEYCODE_1:
		meta.name = "1";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::DIGIT));
		break;
	case EKeycode::KEYCODE_2:
		meta.name = "2";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::DIGIT));
		break;
	case EKeycode::KEYCODE_3:
		meta.name = "3";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::DIGIT));
		break;
	case EKeycode::KEYCODE_4:
		meta.name = "4";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::DIGIT));
		break;
	case EKeycode::KEYCODE_5:
		meta.name = "5";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::DIGIT));
		break;
	case EKeycode::KEYCODE_6:
		meta.name = "6";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::DIGIT));
		break;
	case EKeycode::KEYCODE_7:
		meta.name = "7";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::DIGIT));
		break;
	case EKeycode::KEYCODE_8:
		meta.name = "8";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::DIGIT));
		break;
	case EKeycode::KEYCODE_9:
		meta.name = "9";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::DIGIT));
		break;

	case EKeycode::KEYCODE_F1:
		meta.name = "F1";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F2:
		meta.name = "F2";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F3:
		meta.name = "F3";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F4:
		meta.name = "F4";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F5:
		meta.name = "F5";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F6:
		meta.name = "F6";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F7:
		meta.name = "F7";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F8:
		meta.name = "F8";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F9:
		meta.name = "F9";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F10:
		meta.name = "F10";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F11:
		meta.name = "F11";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F12:
		meta.name = "F12";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F13:
		meta.name = "F13";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F14:
		meta.name = "F14";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F15:
		meta.name = "F15";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F16:
		meta.name = "F16";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F17:
		meta.name = "F17";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F18:
		meta.name = "F18";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F19:
		meta.name = "F19";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F20:
		meta.name = "F20";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F21:
		meta.name = "F21";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F22:
		meta.name = "F22";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F23:
		meta.name = "F23";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;
	case EKeycode::KEYCODE_F24:
		meta.name = "F24";
		meta.group_flags.set(std::to_underlying(EKeyGroup::FUNCTIONAL));
		break;

	case EKeycode::KEYCODE_UP:
		meta.name = "Up";
		break;
	case EKeycode::KEYCODE_DOWN:
		meta.name = "Down";
		break;
	case EKeycode::KEYCODE_LEFT:
		meta.name = "Left";
		break;
	case EKeycode::KEYCODE_RIGHT:
		meta.name = "Right";
		break;
	case EKeycode::KEYCODE_HOME:
		meta.name = "Home";
		break;
	case EKeycode::KEYCODE_END:
		meta.name = "End";
		break;
	case EKeycode::KEYCODE_PAGE_UP:
		meta.name = "PageUp";
		break;
	case EKeycode::KEYCODE_PAGE_DOWN:
		meta.name = "PageDown";
		break;

	case EKeycode::KEYCODE_INSERT:
		meta.name = "Insert";
		break;
	case EKeycode::KEYCODE_DELETE:
		meta.name = "Delete";
		break;
	case EKeycode::KEYCODE_BACKSPACE:
		meta.name = "Backspace";
		break;

	case EKeycode::KEYCODE_ESCAPE:
		meta.name = "Escape";
		break;
	case EKeycode::KEYCODE_TAB:
		meta.name = "Tab";
		break;
	case EKeycode::KEYCODE_CAPS_LOCK:
		meta.name = "CapsLock";
		break;
	case EKeycode::KEYCODE_SPACE:
		meta.name = "Space";
		break;
	case EKeycode::KEYCODE_ENTER:
		meta.name = "Enter";
		break;

	case EKeycode::KEYCODE_LEFT_SHIFT:
		meta.name = "LeftShift";
		break;
	case EKeycode::KEYCODE_RIGHT_SHIFT:
		meta.name = "RightShift";
		break;
	case EKeycode::KEYCODE_LEFT_CTRL:
		meta.name = "LeftCtrl";
		break;
	case EKeycode::KEYCODE_RIGHT_CTRL:
		meta.name = "RightCtrl";
		break;
	case EKeycode::KEYCODE_LEFT_ALT:
		meta.name = "LeftAlt";
		break;
	case EKeycode::KEYCODE_RIGHT_ALT:
		meta.name = "RightAlt";
		break;
	case EKeycode::KEYCODE_LEFT_SUPER:
		meta.name = "LeftSuper";
		break;
	case EKeycode::KEYCODE_RIGHT_SUPER:
		meta.name = "RightSuper";
		break;
	case EKeycode::KEYCODE_MENU:
		meta.name = "Menu";
		break;

	case EKeycode::KEYCODE_NUMPAD_0:
		meta.name = "Numpad0";
		break;
	case EKeycode::KEYCODE_NUMPAD_1:
		meta.name = "Numpad1";
		break;
	case EKeycode::KEYCODE_NUMPAD_2:
		meta.name = "Numpad2";
		break;
	case EKeycode::KEYCODE_NUMPAD_3:
		meta.name = "Numpad3";
		break;
	case EKeycode::KEYCODE_NUMPAD_4:
		meta.name = "Numpad4";
		break;
	case EKeycode::KEYCODE_NUMPAD_5:
		meta.name = "Numpad5";
		break;
	case EKeycode::KEYCODE_NUMPAD_6:
		meta.name = "Numpad6";
		break;
	case EKeycode::KEYCODE_NUMPAD_7:
		meta.name = "Numpad7";
		break;
	case EKeycode::KEYCODE_NUMPAD_8:
		meta.name = "Numpad8";
		break;
	case EKeycode::KEYCODE_NUMPAD_9:
		meta.name = "Numpad9";
		break;
	case EKeycode::KEYCODE_NUMPAD_MULTIPLY:
		meta.name = "Numpad*";
		break;
	case EKeycode::KEYCODE_NUMPAD_ADD:
		meta.name = "Numpad+";
		break;
	case EKeycode::KEYCODE_NUMPAD_SEPARATOR:
		meta.name = "NumpadSeparator";
		break;
	case EKeycode::KEYCODE_NUMPAD_SUBTRACT:
		meta.name = "Numpad-";
		break;
	case EKeycode::KEYCODE_NUMPAD_DECIMAL:
		meta.name = "Numpad.";
		break;
	case EKeycode::KEYCODE_NUMPAD_DIVIDE:
		meta.name = "Numpad/";
		break;
	case EKeycode::KEYCODE_NUMPAD_ENTER:
		meta.name = "NumpadEnter";
		break;

	case EKeycode::KEYCODE_SCROLL_LOCK:
		meta.name = "ScrollLock";
		break;
	case EKeycode::KEYCODE_NUM_LOCK:
		meta.name = "NumLock";
		break;
	case EKeycode::KEYCODE_PAUSE:
		meta.name = "Pause";
		break;

	case EKeycode::KEYCODE_GRAVE:
		meta.name = "`";
		break;
	case EKeycode::KEYCODE_MINUS:
		meta.name = "-";
		break;
	case EKeycode::KEYCODE_EQUALS:
		meta.name = "=";
		break;
	case EKeycode::KEYCODE_LEFT_BRACKET:
		meta.name = "[";
		break;
	case EKeycode::KEYCODE_RIGHT_BRACKET:
		meta.name = "]";
		break;
	case EKeycode::KEYCODE_BACKSLASH:
		meta.name = "\\";
		break;
	case EKeycode::KEYCODE_SEMICOLON:
		meta.name = ";";
		break;
	case EKeycode::KEYCODE_APOSTROPHE:
		meta.name = "'";
		break;
	case EKeycode::KEYCODE_COMMA:
		meta.name = ",";
		break;
	case EKeycode::KEYCODE_PERIOD:
		meta.name = ".";
		break;
	case EKeycode::KEYCODE_SLASH:
		meta.name = "/";
		break;

	case EKeycode::KEYCODE_VOLUME_MUTE:
		meta.name = "VolumeMute";
		break;
	case EKeycode::KEYCODE_VOLUME_DOWN:
		meta.name = "VolumeDown";
		break;
	case EKeycode::KEYCODE_VOLUME_UP:
		meta.name = "VolumeUp";
		break;
	case EKeycode::KEYCODE_MEDIA_NEXT:
		meta.name = "MediaNext";
		break;
	case EKeycode::KEYCODE_MEDIA_PREV:
		meta.name = "MediaPrev";
		break;
	case EKeycode::KEYCODE_MEDIA_STOP:
		meta.name = "MediaStop";
		break;
	case EKeycode::KEYCODE_MEDIA_PLAY_PAUSE:
		meta.name = "MediaPlayPause";
		break;

	case EKeycode::KEYCODE_BROWSER_BACK:
		meta.name = "BrowserBack";
		break;
	case EKeycode::KEYCODE_BROWSER_FORWARD:
		meta.name = "BrowserForward";
		break;
	case EKeycode::KEYCODE_BROWSER_REFRESH:
		meta.name = "BrowserRefresh";
		break;
	case EKeycode::KEYCODE_BROWSER_STOP:
		meta.name = "BrowserStop";
		break;
	case EKeycode::KEYCODE_BROWSER_SEARCH:
		meta.name = "BrowserSearch";
		break;
	case EKeycode::KEYCODE_BROWSER_FAVORITES:
		meta.name = "BrowserFavorites";
		break;
	case EKeycode::KEYCODE_BROWSER_HOME:
		meta.name = "BrowserHome";
		break;

	case EKeycode::KEYCODE_LAUNCH_MAIL:
		meta.name = "LaunchMail";
		break;
	case EKeycode::KEYCODE_LAUNCH_MEDIA_SELECT:
		meta.name = "LaunchMediaSelect";
		break;
	case EKeycode::KEYCODE_LAUNCH_APP1:
		meta.name = "LaunchApp1";
		break;
	case EKeycode::KEYCODE_LAUNCH_APP2:
		meta.name = "LaunchApp2";
		break;

	case EKeycode::KEYCODE_NONE:
		meta.name = "None";
		break;
	case EKeycode::KEYCODE_ANY:
		meta.name = "Any";
		break;

	default:
		meta.name = "Unknown";
		break;
	}

	return meta;
}

using KeyMetaArray = std::array<SKeyMeta, std::to_underlying(EKeycode::KEYCODE_COUNT)>;

[[nodiscard]] static consteval auto InitializeMetaArray() -> KeyMetaArray {
	KeyMetaArray array;
	for (size_t i = 0; i < static_cast<size_t>(EKeycode::KEYCODE_COUNT); ++i) {
		auto meta = InitializeMeta(static_cast<EKeycode>(i));
		array[i] = meta;
	}
	return array;
}

static constexpr KeyMetaArray cKeyMetadata = InitializeMetaArray();

export [[nodiscard]] constexpr auto GetKeycodeName(EKeycode keycode) -> std::string_view {
	auto index = static_cast<size_t>(keycode);
	if (index < 0 || index > cKeyMetadata.size()) [[unlikely]]
		return "unknown";

	return cKeyMetadata[index].name;
}

export [[nodiscard]] constexpr auto IsKeycodeInGroup(EKeycode keycode, EKeyGroup group) -> bool {
	auto index = static_cast<size_t>(keycode);
	if (index < 0 || index > cKeyMetadata.size()) [[unlikely]]
		return false;

	return cKeyMetadata[index].group_flags.test(std::to_underlying(group));
}
