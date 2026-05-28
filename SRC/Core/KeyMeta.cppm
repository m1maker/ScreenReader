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
	LOCKABLE,
	MEDIA,
	APPLICATION,
	COUNT
};

using KeyGroupMask = std::bitset<std::to_underlying(EKeyGroup::COUNT)>;

struct SKeyMeta final {
	std::string_view name{}, modifier_name{};
	KeyGroupMask group_flags;
	EModifier modifier{EModifier::MODIFIER_NONE};
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
		meta.group_flags.set(std::to_underlying(EKeyGroup::NAVIGATIONAL));
		break;
	case EKeycode::KEYCODE_DOWN:
		meta.name = "Down";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NAVIGATIONAL));
		break;
	case EKeycode::KEYCODE_LEFT:
		meta.name = "Left";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NAVIGATIONAL));
		break;
	case EKeycode::KEYCODE_RIGHT:
		meta.name = "Right";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NAVIGATIONAL));
		break;
	case EKeycode::KEYCODE_HOME:
		meta.name = "Home";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NAVIGATIONAL));
		break;
	case EKeycode::KEYCODE_END:
		meta.name = "End";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NAVIGATIONAL));
		break;
	case EKeycode::KEYCODE_PAGE_UP:
		meta.name = "PageUp";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NAVIGATIONAL));
		break;
	case EKeycode::KEYCODE_PAGE_DOWN:
		meta.name = "PageDown";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NAVIGATIONAL));
		break;

	case EKeycode::KEYCODE_INSERT:
		meta.name = "Insert";
		meta.modifier_name = "Insert";
		meta.group_flags.set(std::to_underlying(EKeyGroup::MODIFIER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::EDITING));
		meta.group_flags.set(std::to_underlying(EKeyGroup::LOCKABLE));
		meta.modifier = EModifier::MODIFIER_INSERT;
		break;
	case EKeycode::KEYCODE_DELETE:
		meta.name = "Delete";
		meta.group_flags.set(std::to_underlying(EKeyGroup::EDITING));
		break;
	case EKeycode::KEYCODE_BACKSPACE:
		meta.name = "Backspace";
		meta.group_flags.set(std::to_underlying(EKeyGroup::EDITING));
		break;

	case EKeycode::KEYCODE_ESCAPE:
		meta.name = "Escape";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NAVIGATIONAL));
		break;
	case EKeycode::KEYCODE_TAB:
		meta.name = "Tab";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NAVIGATIONAL));
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		break;
	case EKeycode::KEYCODE_CAPS_LOCK:
		meta.name = "CapsLock";
		meta.modifier_name = "CapsLock";
		meta.group_flags.set(std::to_underlying(EKeyGroup::MODIFIER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::LOCKABLE));
		meta.modifier = EModifier::MODIFIER_CAPS_LOCK;
		break;
	case EKeycode::KEYCODE_SPACE:
		meta.name = "Space";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::NAVIGATIONAL));
		break;
	case EKeycode::KEYCODE_ENTER:
		meta.name = "Enter";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::NAVIGATIONAL));
		break;

	case EKeycode::KEYCODE_LEFT_SHIFT:
		meta.name = "LeftShift";
		meta.modifier_name = "Shift";
		meta.group_flags.set(std::to_underlying(EKeyGroup::MODIFIER));
		meta.modifier = EModifier::MODIFIER_SHIFT;
		break;
	case EKeycode::KEYCODE_RIGHT_SHIFT:
		meta.name = "RightShift";
		meta.modifier_name = "Shift";
		meta.group_flags.set(std::to_underlying(EKeyGroup::MODIFIER));
		meta.modifier = EModifier::MODIFIER_SHIFT;
		break;
	case EKeycode::KEYCODE_LEFT_CTRL:
		meta.name = "LeftCtrl";
		meta.modifier_name = "Ctrl";
		meta.group_flags.set(std::to_underlying(EKeyGroup::MODIFIER));
		meta.modifier = EModifier::MODIFIER_CTRL;
		break;
	case EKeycode::KEYCODE_RIGHT_CTRL:
		meta.name = "RightCtrl";
		meta.modifier_name = "Ctrl";
		meta.group_flags.set(std::to_underlying(EKeyGroup::MODIFIER));
		meta.modifier = EModifier::MODIFIER_CTRL;
		break;
	case EKeycode::KEYCODE_LEFT_ALT:
		meta.name = "LeftAlt";
		meta.modifier_name = "Alt";
		meta.group_flags.set(std::to_underlying(EKeyGroup::MODIFIER));
		meta.modifier = EModifier::MODIFIER_ALT;
		break;
	case EKeycode::KEYCODE_RIGHT_ALT:
		meta.name = "RightAlt";
		meta.modifier_name = "Alt";
		meta.group_flags.set(std::to_underlying(EKeyGroup::MODIFIER));
		meta.modifier = EModifier::MODIFIER_ALT;
		break;
	case EKeycode::KEYCODE_LEFT_SUPER:
		meta.name = "LeftSuper";
		meta.modifier_name = "Super";
		meta.group_flags.set(std::to_underlying(EKeyGroup::MODIFIER));
		meta.modifier = EModifier::MODIFIER_SUPER;
		break;
	case EKeycode::KEYCODE_RIGHT_SUPER:
		meta.name = "RightSuper";
		meta.modifier_name = "Super";
		meta.group_flags.set(std::to_underlying(EKeyGroup::MODIFIER));
		meta.modifier = EModifier::MODIFIER_SUPER;
		break;
	case EKeycode::KEYCODE_MENU:
		meta.name = "Menu";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NAVIGATIONAL));
		meta.group_flags.set(std::to_underlying(EKeyGroup::APPLICATION));
		break;

	case EKeycode::KEYCODE_NUMPAD_0:
		meta.name = "Numpad0";
		meta.modifier_name = "Insert";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NUMPAD));
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::DIGIT));
		meta.group_flags.set(std::to_underlying(EKeyGroup::MODIFIER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::LOCKABLE));
		meta.modifier = MODIFIER_INSERT;
		break;
	case EKeycode::KEYCODE_NUMPAD_1:
		meta.name = "Numpad1";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NUMPAD));
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::DIGIT));
		break;
	case EKeycode::KEYCODE_NUMPAD_2:
		meta.name = "Numpad2";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NUMPAD));
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::DIGIT));
		break;
	case EKeycode::KEYCODE_NUMPAD_3:
		meta.name = "Numpad3";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NUMPAD));
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::DIGIT));
		break;
	case EKeycode::KEYCODE_NUMPAD_4:
		meta.name = "Numpad4";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NUMPAD));
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::DIGIT));
		break;
	case EKeycode::KEYCODE_NUMPAD_5:
		meta.name = "Numpad5";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NUMPAD));
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::DIGIT));
		break;
	case EKeycode::KEYCODE_NUMPAD_6:
		meta.name = "Numpad6";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NUMPAD));
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::DIGIT));
		break;
	case EKeycode::KEYCODE_NUMPAD_7:
		meta.name = "Numpad7";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NUMPAD));
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::DIGIT));
		break;
	case EKeycode::KEYCODE_NUMPAD_8:
		meta.name = "Numpad8";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NUMPAD));
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::DIGIT));
		break;
	case EKeycode::KEYCODE_NUMPAD_9:
		meta.name = "Numpad9";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NUMPAD));
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::DIGIT));
		break;
	case EKeycode::KEYCODE_NUMPAD_MULTIPLY:
		meta.name = "Numpad*";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NUMPAD));
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::PUNCTUATION));
		break;
	case EKeycode::KEYCODE_NUMPAD_ADD:
		meta.name = "Numpad+";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NUMPAD));
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::PUNCTUATION));
		break;
	case EKeycode::KEYCODE_NUMPAD_SEPARATOR:
		meta.name = "NumpadSeparator";
		//.... What?
		break;
	case EKeycode::KEYCODE_NUMPAD_SUBTRACT:
		meta.name = "Numpad-";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NUMPAD));
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::PUNCTUATION));
		break;
	case EKeycode::KEYCODE_NUMPAD_DECIMAL:
		meta.name = "Numpad.";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NUMPAD));
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::PUNCTUATION));
		break;
	case EKeycode::KEYCODE_NUMPAD_DIVIDE:
		meta.name = "Numpad/";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NUMPAD));
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::PUNCTUATION));
		break;
	case EKeycode::KEYCODE_NUMPAD_ENTER:
		meta.name = "NumpadEnter";
		meta.group_flags.set(std::to_underlying(EKeyGroup::NUMPAD));
		meta.group_flags.set(std::to_underlying(EKeyGroup::NAVIGATIONAL));
		break;

	case EKeycode::KEYCODE_SCROLL_LOCK:
		meta.name = "ScrollLock";
		meta.modifier_name = "ScrollLock";
		meta.group_flags.set(std::to_underlying(EKeyGroup::MODIFIER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::LOCKABLE));
		meta.modifier = EModifier::MODIFIER_SCROLL_LOCK;
		break;
	case EKeycode::KEYCODE_NUM_LOCK:
		meta.name = "NumLock";
		meta.modifier_name = "NumLock";
		meta.group_flags.set(std::to_underlying(EKeyGroup::MODIFIER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::LOCKABLE));
		meta.modifier = EModifier::MODIFIER_NUM_LOCK;
		break;
	case EKeycode::KEYCODE_PAUSE:
		meta.name = "Pause";
		meta.group_flags.set(std::to_underlying(EKeyGroup::APPLICATION));
		break;

	case EKeycode::KEYCODE_GRAVE:
		meta.name = "`";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::PUNCTUATION));
		break;
	case EKeycode::KEYCODE_MINUS:
		meta.name = "-";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::PUNCTUATION));
		break;
	case EKeycode::KEYCODE_EQUALS:
		meta.name = "=";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::PUNCTUATION));
		break;
	case EKeycode::KEYCODE_LEFT_BRACKET:
		meta.name = "[";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::PUNCTUATION));
		break;
	case EKeycode::KEYCODE_RIGHT_BRACKET:
		meta.name = "]";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::PUNCTUATION));
		break;
	case EKeycode::KEYCODE_BACKSLASH:
		meta.name = "\\";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::PUNCTUATION));
		break;
	case EKeycode::KEYCODE_SEMICOLON:
		meta.name = ";";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::PUNCTUATION));
		break;
	case EKeycode::KEYCODE_APOSTROPHE:
		meta.name = "'";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::PUNCTUATION));
		break;
	case EKeycode::KEYCODE_COMMA:
		meta.name = ",";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::PUNCTUATION));
		break;
	case EKeycode::KEYCODE_PERIOD:
		meta.name = ".";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::PUNCTUATION));
		break;
	case EKeycode::KEYCODE_SLASH:
		meta.name = "/";
		meta.group_flags.set(std::to_underlying(EKeyGroup::TYPABLE));
		meta.group_flags.set(std::to_underlying(EKeyGroup::CHARACTER));
		meta.group_flags.set(std::to_underlying(EKeyGroup::PUNCTUATION));
		break;

	case EKeycode::KEYCODE_VOLUME_MUTE:
		meta.name = "VolumeMute";
		meta.group_flags.set(std::to_underlying(EKeyGroup::MEDIA));
		break;
	case EKeycode::KEYCODE_VOLUME_DOWN:
		meta.name = "VolumeDown";
		meta.group_flags.set(std::to_underlying(EKeyGroup::MEDIA));
		break;
	case EKeycode::KEYCODE_VOLUME_UP:
		meta.name = "VolumeUp";
		meta.group_flags.set(std::to_underlying(EKeyGroup::MEDIA));
		break;
	case EKeycode::KEYCODE_MEDIA_NEXT:
		meta.name = "MediaNext";
		meta.group_flags.set(std::to_underlying(EKeyGroup::MEDIA));
		break;
	case EKeycode::KEYCODE_MEDIA_PREV:
		meta.name = "MediaPrev";
		meta.group_flags.set(std::to_underlying(EKeyGroup::MEDIA));
		break;
	case EKeycode::KEYCODE_MEDIA_STOP:
		meta.name = "MediaStop";
		meta.group_flags.set(std::to_underlying(EKeyGroup::MEDIA));
		break;
	case EKeycode::KEYCODE_MEDIA_PLAY_PAUSE:
		meta.name = "MediaPlayPause";
		meta.group_flags.set(std::to_underlying(EKeyGroup::MEDIA));
		break;

	case EKeycode::KEYCODE_BROWSER_BACK:
		meta.name = "BrowserBack";
		meta.group_flags.set(std::to_underlying(EKeyGroup::APPLICATION));
		meta.group_flags.set(std::to_underlying(EKeyGroup::NAVIGATIONAL));
		break;
	case EKeycode::KEYCODE_BROWSER_FORWARD:
		meta.name = "BrowserForward";
		meta.group_flags.set(std::to_underlying(EKeyGroup::APPLICATION));
		meta.group_flags.set(std::to_underlying(EKeyGroup::NAVIGATIONAL));
		break;
	case EKeycode::KEYCODE_BROWSER_REFRESH:
		meta.name = "BrowserRefresh";
		meta.group_flags.set(std::to_underlying(EKeyGroup::APPLICATION));
		break;
	case EKeycode::KEYCODE_BROWSER_STOP:
		meta.name = "BrowserStop";
		meta.group_flags.set(std::to_underlying(EKeyGroup::APPLICATION));
		break;
	case EKeycode::KEYCODE_BROWSER_SEARCH:
		meta.name = "BrowserSearch";
		meta.group_flags.set(std::to_underlying(EKeyGroup::APPLICATION));
		meta.group_flags.set(std::to_underlying(EKeyGroup::NAVIGATIONAL));
		break;
	case EKeycode::KEYCODE_BROWSER_FAVORITES:
		meta.name = "BrowserFavorites";
		meta.group_flags.set(std::to_underlying(EKeyGroup::APPLICATION));
		meta.group_flags.set(std::to_underlying(EKeyGroup::NAVIGATIONAL));
		break;
	case EKeycode::KEYCODE_BROWSER_HOME:
		meta.name = "BrowserHome";
		meta.group_flags.set(std::to_underlying(EKeyGroup::APPLICATION));
		meta.group_flags.set(std::to_underlying(EKeyGroup::NAVIGATIONAL));
		break;

	case EKeycode::KEYCODE_LAUNCH_MAIL:
		meta.name = "LaunchMail";
		meta.group_flags.set(std::to_underlying(EKeyGroup::APPLICATION));
		break;
	case EKeycode::KEYCODE_LAUNCH_MEDIA_SELECT:
		meta.name = "LaunchMediaSelect";
		meta.group_flags.set(std::to_underlying(EKeyGroup::APPLICATION));
		break;
	case EKeycode::KEYCODE_LAUNCH_APP1:
		meta.name = "LaunchApp1";
		meta.group_flags.set(std::to_underlying(EKeyGroup::APPLICATION));
		break;
	case EKeycode::KEYCODE_LAUNCH_APP2:
		meta.name = "LaunchApp2";
		meta.group_flags.set(std::to_underlying(EKeyGroup::APPLICATION));
		break;

	case EKeycode::KEYCODE_NONE:
	case EKeycode::KEYCODE_COUNT:
		meta.name = "None";
		break;
	case EKeycode::KEYCODE_ANY:
		meta.name = "Any";
		meta.group_flags.set(std::to_underlying(EKeyGroup::UNPREDICTABLE));
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

export [[nodiscard]] constexpr auto GetModifierName(EKeycode keycode) -> std::string_view {
	auto index = static_cast<size_t>(keycode);
	if (index < 0 || index > cKeyMetadata.size()) [[unlikely]]
		return "unknown";

	return cKeyMetadata[index].modifier_name;
}

export [[nodiscard]] constexpr auto IsKeycodeInGroup(EKeycode keycode, EKeyGroup group) -> bool {
	auto index = static_cast<size_t>(keycode);
	if (index < 0 || index > cKeyMetadata.size()) [[unlikely]]
		return false;

	return cKeyMetadata[index].group_flags.test(std::to_underlying(group));
}

export [[nodiscard]] constexpr auto GetModifierFromKeycode(EKeycode keycode) -> EModifier {
	auto index = static_cast<size_t>(keycode);
	if (index < 0 || index > cKeyMetadata.size()) [[unlikely]]
		return EModifier::MODIFIER_NONE;

	return cKeyMetadata[index].modifier;
}
