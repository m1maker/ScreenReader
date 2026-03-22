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
import Core.Object;
import Core.SpeechSystem;

EventHandler::EventHandler()
	: m_focusManager(CFocusManager::GetInstance()), m_eventQueue(EventQueue::GetInstance()),
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

void EventHandler::Start() {
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
							EventHandler::GetInstance().Handle(std::move(*event_casted));
							auto pool = EventQueue::GetInstance().GetPool();
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
void EventHandler::Handle(CEvent&& event) {
	LogCalled();

	try {
		switch (event.GetType()) {
		case CEvent::OBJECT: {
			auto object_event = event.GetAs<CObjectEvent>();
			if (!object_event.has_value())
				break;

			auto evt = object_event.value();
			auto& settings = ScreenReaderApp::GetInstance().GetSettings();
			if (settings.object_presentation.read_unfocused_object_changes && m_focusManager.GetFocus() != evt.object &&
				evt.type != EObjectEventType::FOCUS_GAINED) {
				m_eventToSpeech.AnnounceFocusChange(evt.object);
				return;
			}
			switch (evt.type) {
			case EObjectEventType::FOCUS_GAINED:
				m_focusManager.SetFocus(evt.object);
				CSpeechSystem::GetInstance().Stop();
				m_eventToSpeech.AnnounceWhereAmI();
				m_eventToSpeech.AnnounceFocusChange(evt.object, false);
				break;
			case EObjectEventType::PARENT_UPDATED:
				m_eventToSpeech.AnnounceWhereAmI();
				break;
			case EObjectEventType::VALUE_CHANGED:
				m_eventToSpeech.AnnounceValueChange(evt.object, m_focusManager.GetFocus() == evt.object);
				break;
			case EObjectEventType::STATE_CHANGED:
				m_eventToSpeech.AnnounceStateChange(evt.object, m_focusManager.GetFocus() == evt.object);
				break;
			case EObjectEventType::SELECTION_CHANGED:
				m_eventToSpeech.AnnounceSelectionChange(evt.object);
				break;
			case EObjectEventType::CURSOR_MOVED:
				m_eventToSpeech.AnnounceCursorMove(evt.object, m_focusManager.GetFocus() == evt.object);
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
