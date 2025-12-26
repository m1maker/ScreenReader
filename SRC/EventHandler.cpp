// Handling events of different types.
#include "EventHandler.h"
#include "EventListenerAtspi.h"
#include "Logger.h"
#include "EventToSpeech.h"
#include "KeyboardHandler.h"
#include "Action.h"
#include "FocusManager.h"

CEventHandler::CEventHandler() {
	m_listener = std::make_shared<CEventListenerAtspi>(); // In the future, this will of course be platform specific.

	bool success{false};
	success = g_keyboardHandler.RegisterAction(CKeyboardEvent::SHotkeyInfo::GetAny(), static_cast<unsigned int>(EAction::STOP_SPEECH), g_actionStopSpeech(CKeyboardEvent::SHotkeyInfo));
	//g_eventToSpeech.AnnounceWhereAmI();
}

/*
This function is called by a platform-specific event listener when it has been typecast to IEvent::EEventType and dispatched with the desired event type category.
*/
void CEventHandler::Handle() {
	[[maybe_unused]] CScopedCategory _("Event handler");
	if (!m_listener) [[unlikely]] {
		g_logger.Log(CLogger::ERROR, "There is no event listener registered for this platform");
		return;
	}

	auto& event_queue = m_listener->RequestQueue();
	if (event_queue.empty()) [[unlikely]] {
		g_logger.Log(CLogger::WARNING, "The event listener was called the handler, but there are no events to handle");
		return;
	}

	for (auto& event : event_queue) {
		switch (event.GetType()) {
			case CEvent::FOCUS_GAINED:
			case CEvent::PARENT_UPDATED: {
				auto object_event = event.GetAs<CObjectEvent>();
				if (!object_event.has_value()) break;
				g_focusManager.m_objectInFocus = object_event.value().object;
				g_eventToSpeech.AnnounceFocusChange(event);
				break;
			}
			case CEvent::VALUE_CHANGED: {
				g_eventToSpeech.AnnounceValueChange(event);
				break;
			}
			case CEvent::STATE_CHANGED: {
				g_eventToSpeech.AnnounceStateChange(event);
				break;
			}
			case CEvent::SELECTION_CHANGED: {
				g_eventToSpeech.AnnounceSelectionChange(event);
				break;
			}

			case CEvent::CURSOR_MOVED: {
				g_eventToSpeech.AnnounceCursorMove(event);
				break;
			}

			case CEvent::KEY_PRESSED: {
				auto keyboard_event = event.GetAs<CKeyboardEvent>();
				if (!keyboard_event.has_value()) break;
				g_keyboardHandler.m_keysDown[keyboard_event.value().hotkey.keycode] = true;
				g_keyboardHandler.m_modifiers |= keyboard_event.value().hotkey.modifiers;

				g_keyboardHandler.Handle(keyboard_event.value());
				break;
			}
			case CEvent::KEY_RELEASED: {
				auto keyboard_event = event.GetAs<CKeyboardEvent>();
				if (!keyboard_event.has_value()) break;
				g_keyboardHandler.m_keysDown[keyboard_event.value().hotkey.keycode] = false;
				g_keyboardHandler.m_modifiers &= ~keyboard_event.value().hotkey.modifiers;

				break;
			}
			default:
				break;
		}
	}

	event_queue.clear();
}
