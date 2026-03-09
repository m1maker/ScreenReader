// Handling events of different types.
module;
#include "Logger.h"

#include <expected>
#include <thread>
module Core.EventHandler;
import Core.Action;
import Core.App;
import Core.AppState;
import Core.Device;
import Core.Event;
import Core.KeyboardHandler;
import Core.SpeechSystem;
import Traits.Object;

CEventHandler::CEventHandler()
	: m_focusManager(CFocusManager::GetInstance()), m_eventQueue(CEventQueue::GetInstance()),
	  m_eventToSpeech(CEventToSpeech::GetInstance()) {
	m_listener.ListenDevice(EDeviceType::KEYBOARD);
	auto& keyboard_handler = CKeyboardHandler::GetInstance();

	bool success{false};
	success = keyboard_handler.RegisterAction(SHotkeyInfo::GetAny(), static_cast<uint32_t>(EAction::STOP_SPEECH));
	success = keyboard_handler.RegisterAction(
		CKeyboardEvent::MODIFIER_SCREEN_READER + CKeyboardEvent::MODIFIER_CTRL + CKeyboardEvent::KEYCODE_K,
		static_cast<uint32_t>(EAction::STOP_KEYBOARD_HOOKS),
		true);

	m_eventToSpeech.AnnounceWhereAmI();
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
				CSpeechSystem::GetInstance().Stop();
				m_eventToSpeech.AnnounceFocusChange(event);
				break;
			case EObjectEventType::PARENT_UPDATED:
				m_eventToSpeech.AnnounceWhereAmI();
				break;
			case EObjectEventType::VALUE_CHANGED:
				m_eventToSpeech.AnnounceValueChange(event);
				break;
			case EObjectEventType::STATE_CHANGED:
				m_eventToSpeech.AnnounceStateChange(event);
				break;
			case EObjectEventType::SELECTION_CHANGED:
				m_eventToSpeech.AnnounceSelectionChange(event);
				break;
			case EObjectEventType::CURSOR_MOVED:
				m_eventToSpeech.AnnounceCursorMove(event);
				break;
			default:
				break;
			}
		}

		case CEvent::KEYBOARD: {
			auto& keyboard_handler = CKeyboardHandler::GetInstance();
			std::scoped_lock lock(keyboard_handler.m_mutex);
			auto keyboard_event = event.GetAs<CKeyboardEvent>();
			if (!keyboard_event.has_value())
				break;

			switch (keyboard_event.value().type) {
			case CKeyboardEvent::KEY_PRESSED:
				keyboard_handler.m_keysDown[keyboard_event.value().hotkey.keycode] = true;
				keyboard_handler.m_modifiers = keyboard_event.value().hotkey.modifiers;

				keyboard_handler.Handle(keyboard_event.value());
				break;
			case CKeyboardEvent::KEY_RELEASED:
				keyboard_handler.m_keysDown[keyboard_event.value().hotkey.keycode] = false;
				keyboard_handler.m_modifiers = keyboard_event.value().hotkey.modifiers;

				break;
			default:
				break;
			}
		}
		default:
			break;
		}
	}
	catch (const std::bad_expected_access<EObjectError>& error) {
		g_logger.Log(CLogger::ERROR, "Expected access error: " + std::string(ObjectErrorToString(error.error())));
	}
}
