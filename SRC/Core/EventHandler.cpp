// Handling events of different types.
module;
#include "Event.h"
#include "EventToSpeech.h"
#include "KeyboardHandler.h"
#include "Logger.h"
#include "SpeechEngine.h"

#include <thread>
module Core.EventHandler;
import Core.Action;
import Core.App;
import Core.AppState;
import Core.Device;

CEventHandler::CEventHandler()
	: m_focusManager(CFocusManager::GetInstance()), m_eventQueue(CEventQueue::GetInstance()) {
	m_listener.ListenDevice(EDeviceType::KEYBOARD);
	bool success{false};
	success = g_keyboardHandler.RegisterAction(SHotkeyInfo::GetAny(), static_cast<uint32_t>(EAction::STOP_SPEECH));
	success = g_keyboardHandler.RegisterAction(
		CKeyboardEvent::MODIFIER_SCREEN_READER + CKeyboardEvent::MODIFIER_CTRL + CKeyboardEvent::KEYCODE_K,
		static_cast<uint32_t>(EAction::STOP_KEYBOARD_HOOKS));

	g_eventToSpeech.AnnounceWhereAmI();
}

void CEventHandler::Start() {
	m_thread = std::jthread([this]() -> void {
		while (g_running.load()) {
			auto event = m_eventQueue.Pop();
			if (event) [[likely]] {
				switch (event.value().GetType()) {
				case CEvent::OBJECT: {
					auto pool = m_eventQueue.GetPool();
					if (!pool) [[unlikely]]
						break;

					auto raw = pool->allocate(sizeof(CEvent));
					auto raw_event = new (raw) CEvent(std::move(event.value()));
					m_listener.PushToMainThread(
						[](void* pData) -> void {
							if (!pData)
								return;
							auto event_casted = static_cast<CEvent*>(pData);
							CEventHandler::GetInstance().Handle(std::move(*event_casted));
							auto pool = CEventQueue::GetInstance().GetPool();
							if (!pool) [[unlikely]]
								return;

							event_casted->~CEvent();
							pool->deallocate(pData, sizeof(CEvent));
						},
						raw_event);
					break;
				}

				case CEvent::KEYBOARD:
					Handle(std::move(event.value()));
					break;
				default:
					break;
				}
			}
		}
	});
}

/*
This function is called by an event handler when it has been typecast to IEvent::EEventType by IEventListener and
dispatched with the desired event type category.
*/
void CEventHandler::Handle(CEvent&& event) {
	LogCalled();

	try {
		switch (event.GetType()) {
		case CEvent::OBJECT: {
			auto object_event = event.GetAs<CObjectEvent>();
			if (!object_event.has_value())
				break;

			switch (object_event.value().type) {
			case EObjectEventType::FOCUS_GAINED:
				m_focusManager.SetFocus(object_event.value().object);
				g_speechEngine.Stop();
				g_eventToSpeech.AnnounceFocusChange(event);
				break;
			case EObjectEventType::PARENT_UPDATED:
				g_eventToSpeech.AnnounceWhereAmI();
				break;
			case EObjectEventType::VALUE_CHANGED:
				g_eventToSpeech.AnnounceValueChange(event);
				break;
			case EObjectEventType::STATE_CHANGED:
				g_eventToSpeech.AnnounceStateChange(event);
				break;
			case EObjectEventType::SELECTION_CHANGED:
				g_eventToSpeech.AnnounceSelectionChange(event);
				break;
			case EObjectEventType::CURSOR_MOVED:
				g_eventToSpeech.AnnounceCursorMove(event);
				break;
			default:
				break;
			}
		}

		case CEvent::KEYBOARD: {
			std::scoped_lock lock(g_keyboardHandler.m_mutex);
			auto keyboard_event = event.GetAs<CKeyboardEvent>();
			if (!keyboard_event.has_value())
				break;

			switch (keyboard_event.value().type) {
			case CKeyboardEvent::KEY_PRESSED:
				g_keyboardHandler.m_keysDown[keyboard_event.value().hotkey.keycode] = true;
				g_keyboardHandler.m_modifiers = keyboard_event.value().hotkey.modifiers;

				g_keyboardHandler.Handle(keyboard_event.value());
				break;
			case CKeyboardEvent::KEY_RELEASED:
				g_keyboardHandler.m_keysDown[keyboard_event.value().hotkey.keycode] = false;
				g_keyboardHandler.m_modifiers = keyboard_event.value().hotkey.modifiers;

				break;
			default:
				break;
			}
		}
		default:
			break;
		}
	}

	catch (const Sral::Exception& speech_exception) {
		g_logger.Log(CLogger::ERROR, speech_exception.what());
	}
	catch (const std::bad_expected_access<EObjectError>& error) {
		g_logger.Log(CLogger::ERROR, "Expected access error: " + std::string(ObjectErrorToString(error.error())));
	}
}
