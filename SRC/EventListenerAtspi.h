// AT_SPI's event listener.
#pragma once
#include "EventListener.h"
#include <atspi/atspi.h>
#include <atspi/atspi-device.h>
#include <gdk/gdkkeysyms.h>
#include <memory>
#include <unordered_map>
#include <string>
#include <linux/input-event-codes.h>
#include <array>

/*
AT-SPI has a listener where you need to register the required events one by one.
But there's one very strange thing: all of these types are strings. Look at this code.
*/
inline const std::unordered_map<std::string, IEvent::EEventType> cAtspiObjectEventTypeMap = {
	{"object:state-changed:focused", IEvent::FOCUS_GAINED},
	{"object:state-changed:checked", IEvent::STATE_CHANGED},
	{"object:state-changed:selected", IEvent::SELECTION_CHANGED},
	{"object:property-change:accessible-parent", IEvent::PARENT_UPDATED},
	{"object:property-change:accessible-value", IEvent::VALUE_CHANGED},
	{"object:text-caret-moved", IEvent::CURSOR_MOVED}
};

[[nodiscard]] constexpr inline IEvent::EEventType GetEventTypeFromString(gchar* type) {
	if (!type) [[unlikely]] return IEvent::NONE;

	std::string type_str(type);

	auto it = cAtspiObjectEventTypeMap.find(type_str);
	if (it != cAtspiObjectEventTypeMap.end()) {
		return it->second;
	}

	return IEvent::NONE;
}

[[nodiscard]] constexpr inline CKeyboardEvent::EKeycode GdkKeysymToKeyboardEventKeycode(const uint32_t& gdk_keysym) {
	switch (gdk_keysym) {
		case GDK_KEY_F1: return CKeyboardEvent::KEYCODE_F1;
		case GDK_KEY_F2: return CKeyboardEvent::KEYCODE_F2;
		case GDK_KEY_F3: return CKeyboardEvent::KEYCODE_F3;
		case GDK_KEY_F4: return CKeyboardEvent::KEYCODE_F4;
		case GDK_KEY_F5: return CKeyboardEvent::KEYCODE_F5;
		case GDK_KEY_F6: return CKeyboardEvent::KEYCODE_F6;
		case GDK_KEY_F7: return CKeyboardEvent::KEYCODE_F7;
		case GDK_KEY_F8: return CKeyboardEvent::KEYCODE_F8;
		case GDK_KEY_F9: return CKeyboardEvent::KEYCODE_F9;
		case GDK_KEY_F10: return CKeyboardEvent::KEYCODE_F10;
		case GDK_KEY_F11: return CKeyboardEvent::KEYCODE_F11;
		case GDK_KEY_F12: return CKeyboardEvent::KEYCODE_F12;
		case GDK_KEY_F13: return CKeyboardEvent::KEYCODE_F13;
		case GDK_KEY_F14: return CKeyboardEvent::KEYCODE_F14;
		case GDK_KEY_F15: return CKeyboardEvent::KEYCODE_F15;
		case GDK_KEY_F16: return CKeyboardEvent::KEYCODE_F16;
		case GDK_KEY_F17: return CKeyboardEvent::KEYCODE_F17;
		case GDK_KEY_F18: return CKeyboardEvent::KEYCODE_F18;
		case GDK_KEY_F19: return CKeyboardEvent::KEYCODE_F19;
		case GDK_KEY_F20: return CKeyboardEvent::KEYCODE_F20;
		case GDK_KEY_F21: return CKeyboardEvent::KEYCODE_F21;
		case GDK_KEY_F22: return CKeyboardEvent::KEYCODE_F22;
		case GDK_KEY_F23: return CKeyboardEvent::KEYCODE_F23;
		case GDK_KEY_F24: return CKeyboardEvent::KEYCODE_F24;

		case GDK_KEY_Up: return CKeyboardEvent::KEYCODE_UP;
		case GDK_KEY_Down: return CKeyboardEvent::KEYCODE_DOWN;
		case GDK_KEY_Left: return CKeyboardEvent::KEYCODE_LEFT;
		case GDK_KEY_Right: return CKeyboardEvent::KEYCODE_RIGHT;
		case GDK_KEY_Home: return CKeyboardEvent::KEYCODE_HOME;
		case GDK_KEY_End: return CKeyboardEvent::KEYCODE_END;
		case GDK_KEY_Page_Up: return CKeyboardEvent::KEYCODE_PAGE_UP;
		case GDK_KEY_Page_Down: return CKeyboardEvent::KEYCODE_PAGE_DOWN;
		case GDK_KEY_Insert: return CKeyboardEvent::KEYCODE_INSERT;
		case GDK_KEY_Delete: return CKeyboardEvent::KEYCODE_DELETE;

		case GDK_KEY_Escape: return CKeyboardEvent::KEYCODE_ESCAPE;
		case GDK_KEY_Tab: return CKeyboardEvent::KEYCODE_TAB;
		case GDK_KEY_Caps_Lock: return CKeyboardEvent::KEYCODE_CAPS_LOCK;
		case GDK_KEY_Shift_L: return CKeyboardEvent::KEYCODE_LEFT_SHIFT;
		case GDK_KEY_Shift_R: return CKeyboardEvent::KEYCODE_RIGHT_SHIFT;
		case GDK_KEY_Control_L: return CKeyboardEvent::KEYCODE_LEFT_CTRL;
		case GDK_KEY_Control_R: return CKeyboardEvent::KEYCODE_RIGHT_CTRL;
		case GDK_KEY_Alt_L: return CKeyboardEvent::KEYCODE_LEFT_ALT;
		case GDK_KEY_Alt_R: return CKeyboardEvent::KEYCODE_RIGHT_ALT;
		case GDK_KEY_Super_L: return CKeyboardEvent::KEYCODE_LEFT_SUPER;
		case GDK_KEY_Super_R: return CKeyboardEvent::KEYCODE_RIGHT_SUPER;
		case GDK_KEY_Menu: return CKeyboardEvent::KEYCODE_MENU;
		case GDK_KEY_space: return CKeyboardEvent::KEYCODE_SPACE;
		case GDK_KEY_Return: return CKeyboardEvent::KEYCODE_ENTER;
		case GDK_KEY_BackSpace: return CKeyboardEvent::KEYCODE_BACKSPACE;

		case GDK_KEY_KP_0: return CKeyboardEvent::KEYCODE_NUMPAD_0;
		case GDK_KEY_KP_1: return CKeyboardEvent::KEYCODE_NUMPAD_1;
		case GDK_KEY_KP_2: return CKeyboardEvent::KEYCODE_NUMPAD_2;
		case GDK_KEY_KP_3: return CKeyboardEvent::KEYCODE_NUMPAD_3;
		case GDK_KEY_KP_4: return CKeyboardEvent::KEYCODE_NUMPAD_4;
		case GDK_KEY_KP_5: return CKeyboardEvent::KEYCODE_NUMPAD_5;
		case GDK_KEY_KP_6: return CKeyboardEvent::KEYCODE_NUMPAD_6;
		case GDK_KEY_KP_7: return CKeyboardEvent::KEYCODE_NUMPAD_7;
		case GDK_KEY_KP_8: return CKeyboardEvent::KEYCODE_NUMPAD_8;
		case GDK_KEY_KP_9: return CKeyboardEvent::KEYCODE_NUMPAD_9;
		case GDK_KEY_KP_Multiply: return CKeyboardEvent::KEYCODE_NUMPAD_MULTIPLY;
		case GDK_KEY_KP_Add: return CKeyboardEvent::KEYCODE_NUMPAD_ADD;
		case GDK_KEY_KP_Separator: return CKeyboardEvent::KEYCODE_NUMPAD_SEPARATOR;
		case GDK_KEY_KP_Subtract: return CKeyboardEvent::KEYCODE_NUMPAD_SUBTRACT;
		case GDK_KEY_KP_Decimal: return CKeyboardEvent::KEYCODE_NUMPAD_DECIMAL;
		case GDK_KEY_KP_Divide: return CKeyboardEvent::KEYCODE_NUMPAD_DIVIDE;
		case GDK_KEY_KP_Enter: return CKeyboardEvent::KEYCODE_NUMPAD_ENTER;

		case GDK_KEY_Scroll_Lock: return CKeyboardEvent::KEYCODE_SCROLL_LOCK;
		case GDK_KEY_Num_Lock: return CKeyboardEvent::KEYCODE_NUM_LOCK;
		case GDK_KEY_Pause: return CKeyboardEvent::KEYCODE_PAUSE;

		case GDK_KEY_grave:
		case GDK_KEY_asciitilde: return CKeyboardEvent::KEYCODE_GRAVE;
		case GDK_KEY_minus:
		case GDK_KEY_underscore: return CKeyboardEvent::KEYCODE_MINUS;
		case GDK_KEY_equal:
		case GDK_KEY_plus: return CKeyboardEvent::KEYCODE_EQUALS;
		case GDK_KEY_bracketleft:
		case GDK_KEY_braceleft: return CKeyboardEvent::KEYCODE_LEFT_BRACKET;
		case GDK_KEY_bracketright:
		case GDK_KEY_braceright: return CKeyboardEvent::KEYCODE_RIGHT_BRACKET;
		case GDK_KEY_backslash:
		case GDK_KEY_bar: return CKeyboardEvent::KEYCODE_BACKSLASH;
		case GDK_KEY_semicolon:
		case GDK_KEY_colon: return CKeyboardEvent::KEYCODE_SEMICOLON;
		case GDK_KEY_apostrophe:
		case GDK_KEY_quotedbl: return CKeyboardEvent::KEYCODE_APOSTROPHE;
		case GDK_KEY_comma:
		case GDK_KEY_less: return CKeyboardEvent::KEYCODE_COMMA;
		case GDK_KEY_period:
		case GDK_KEY_greater: return CKeyboardEvent::KEYCODE_PERIOD;
		case GDK_KEY_slash:
		case GDK_KEY_question: return CKeyboardEvent::KEYCODE_SLASH;

		case GDK_KEY_AudioMute: return CKeyboardEvent::KEYCODE_VOLUME_MUTE;
		case GDK_KEY_AudioLowerVolume: return CKeyboardEvent::KEYCODE_VOLUME_DOWN;
		case GDK_KEY_AudioRaiseVolume: return CKeyboardEvent::KEYCODE_VOLUME_UP;
		case GDK_KEY_AudioNext: return CKeyboardEvent::KEYCODE_MEDIA_NEXT;
		case GDK_KEY_AudioPrev: return CKeyboardEvent::KEYCODE_MEDIA_PREV;
		case GDK_KEY_AudioStop: return CKeyboardEvent::KEYCODE_MEDIA_STOP;
		case GDK_KEY_AudioPlay: return CKeyboardEvent::KEYCODE_MEDIA_PLAY_PAUSE;

		case GDK_KEY_Back: return CKeyboardEvent::KEYCODE_BROWSER_BACK;
		case GDK_KEY_Forward: return CKeyboardEvent::KEYCODE_BROWSER_FORWARD;
		case GDK_KEY_Refresh: return CKeyboardEvent::KEYCODE_BROWSER_REFRESH;
		case GDK_KEY_Stop: return CKeyboardEvent::KEYCODE_BROWSER_STOP;
		case GDK_KEY_Search: return CKeyboardEvent::KEYCODE_BROWSER_SEARCH;
		case GDK_KEY_Favorites: return CKeyboardEvent::KEYCODE_BROWSER_FAVORITES;
		case GDK_KEY_HomePage: return CKeyboardEvent::KEYCODE_BROWSER_HOME;

		case GDK_KEY_Mail: return CKeyboardEvent::KEYCODE_LAUNCH_MAIL;
		case GDK_KEY_Launch0: 
		case GDK_KEY_Launch1: return CKeyboardEvent::KEYCODE_LAUNCH_APP1;
		case GDK_KEY_Launch2: 
		case GDK_KEY_Launch3: return CKeyboardEvent::KEYCODE_LAUNCH_APP2;

		case GDK_KEY_a:
		case GDK_KEY_A: return CKeyboardEvent::KEYCODE_A;
		case GDK_KEY_b:
		case GDK_KEY_B: return CKeyboardEvent::KEYCODE_B;
		case GDK_KEY_c:
		case GDK_KEY_C: return CKeyboardEvent::KEYCODE_C;
		case GDK_KEY_d:
		case GDK_KEY_D: return CKeyboardEvent::KEYCODE_D;
		case GDK_KEY_e:
		case GDK_KEY_E: return CKeyboardEvent::KEYCODE_E;
		case GDK_KEY_f:
		case GDK_KEY_F: return CKeyboardEvent::KEYCODE_F;
		case GDK_KEY_g:
		case GDK_KEY_G: return CKeyboardEvent::KEYCODE_G;
		case GDK_KEY_h:
		case GDK_KEY_H: return CKeyboardEvent::KEYCODE_H;
		case GDK_KEY_i:
		case GDK_KEY_I: return CKeyboardEvent::KEYCODE_I;
		case GDK_KEY_j:
		case GDK_KEY_J: return CKeyboardEvent::KEYCODE_J;
		case GDK_KEY_k:
		case GDK_KEY_K: return CKeyboardEvent::KEYCODE_K;
		case GDK_KEY_l:
		case GDK_KEY_L: return CKeyboardEvent::KEYCODE_L;
		case GDK_KEY_m:
		case GDK_KEY_M: return CKeyboardEvent::KEYCODE_M;
		case GDK_KEY_n:
		case GDK_KEY_N: return CKeyboardEvent::KEYCODE_N;
		case GDK_KEY_o:
		case GDK_KEY_O: return CKeyboardEvent::KEYCODE_O;
		case GDK_KEY_p:
		case GDK_KEY_P: return CKeyboardEvent::KEYCODE_P;
		case GDK_KEY_q:
		case GDK_KEY_Q: return CKeyboardEvent::KEYCODE_Q;
		case GDK_KEY_r:
		case GDK_KEY_R: return CKeyboardEvent::KEYCODE_R;
		case GDK_KEY_s:
		case GDK_KEY_S: return CKeyboardEvent::KEYCODE_S;
		case GDK_KEY_t:
		case GDK_KEY_T: return CKeyboardEvent::KEYCODE_T;
		case GDK_KEY_u:
		case GDK_KEY_U: return CKeyboardEvent::KEYCODE_U;
		case GDK_KEY_v:
		case GDK_KEY_V: return CKeyboardEvent::KEYCODE_V;
		case GDK_KEY_w:
		case GDK_KEY_W: return CKeyboardEvent::KEYCODE_W;
		case GDK_KEY_x:
		case GDK_KEY_X: return CKeyboardEvent::KEYCODE_X;
		case GDK_KEY_y:
		case GDK_KEY_Y: return CKeyboardEvent::KEYCODE_Y;
		case GDK_KEY_z:
		case GDK_KEY_Z: return CKeyboardEvent::KEYCODE_Z;

		case GDK_KEY_0:
		case GDK_KEY_parenright: return CKeyboardEvent::KEYCODE_0;
		case GDK_KEY_1:
		case GDK_KEY_exclam: return CKeyboardEvent::KEYCODE_1;
		case GDK_KEY_2:
		case GDK_KEY_at: return CKeyboardEvent::KEYCODE_2;
		case GDK_KEY_3:
		case GDK_KEY_numbersign: return CKeyboardEvent::KEYCODE_3;
		case GDK_KEY_4:
		case GDK_KEY_dollar: return CKeyboardEvent::KEYCODE_4;
		case GDK_KEY_5:
		case GDK_KEY_percent: return CKeyboardEvent::KEYCODE_5;
		case GDK_KEY_6:
		case GDK_KEY_asciicircum: return CKeyboardEvent::KEYCODE_6;
		case GDK_KEY_7:
		case GDK_KEY_ampersand: return CKeyboardEvent::KEYCODE_7;
		case GDK_KEY_8:
		case GDK_KEY_asterisk: return CKeyboardEvent::KEYCODE_8;
		case GDK_KEY_9:
		case GDK_KEY_parenleft: return CKeyboardEvent::KEYCODE_9;

		default: return CKeyboardEvent::KEYCODE_NONE;
	}
}

[[nodiscard]] constexpr inline IEvent::EEventType AtspiEventTypeToEventType(const AtspiEventType& type) {
	switch (type) {
		case ATSPI_KEY_PRESSED_EVENT:
		case ATSPI_BUTTON_PRESSED_EVENT:
			return IEvent::KEY_PRESSED;
		default: return IEvent::KEY_RELEASED;
	}
}

class CEventListenerAtspi final : public IEventListener {
	AtspiEventListener* m_objectEventListener{nullptr};
	AtspiDevice* m_device{nullptr};
public:

	static void OnObjectEventCallback(AtspiEvent* event, void* user_data);
	static void OnDeviceKeyEventCallback([[maybe_unused]] AtspiDevice* device, gboolean pressed, guint keycode, [[maybe_unused]] guint keysym, guint modifiers, [[maybe_unused]] const gchar* key_string, void* user_data);

	explicit CEventListenerAtspi();
	~CEventListenerAtspi() {
		if (m_objectEventListener) g_object_unref(m_objectEventListener);
		if (m_device) g_object_unref(m_device);
	}

	void Post(std::shared_ptr<IEvent> event) override;

	[[nodiscard]] EventQueue& RequestQueue() override;
};

