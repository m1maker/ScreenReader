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
import Core.KeyInfo;
import Core.Object;
import Core.SpeechSystem;

EventHandler::EventHandler()
	: m_focusManager(FocusManager::GetInstance()), m_eventQueue(EventQueue::GetInstance()),
	  m_outputManager(OutputManager::GetInstance()) {
	m_listener.ListenDevice(EDeviceType::KEYBOARD);
	auto& keyboard_handler = KeyboardHandler::GetInstance();

	bool success{false};
	success = keyboard_handler.RegisterAction(SHotkeyInfo::GetAny(), static_cast<uint32_t>(EAction::STOP_SPEECH));
	success = keyboard_handler.RegisterAction(
		MODIFIER_SCREEN_READER + MODIFIER_CTRL + KEYCODE_K, static_cast<uint32_t>(EAction::STOP_KEYBOARD_HOOKS), true);

	m_outputManager.WhereAmI();
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
				m_outputManager.FocusChange(evt.object);
				return;
			}
			if (m_focusManager.GetFocus() == evt.object)
				m_outputManager.Stop();
			switch (evt.type) {
			case EObjectEventType::FOCUS_GAINED:
				m_focusManager.SetFocus(evt.object);
				m_outputManager.Stop();
				m_outputManager.WhereAmI();
				m_outputManager.FocusChange(evt.object);
				break;
			case EObjectEventType::PARENT_UPDATED:
				m_outputManager.WhereAmI();
				break;
			case EObjectEventType::VALUE_CHANGED:
				m_outputManager.ValueChange(evt.object);
				break;
			case EObjectEventType::STATE_CHANGED:
				m_outputManager.StateChange(evt.object);
				break;
			case EObjectEventType::SELECTION_CHANGED:
				// m_outputManager.SelectionChange(evt.object);
				break;
			case EObjectEventType::CURSOR_MOVED:
				m_outputManager.CursorMove(evt.object);
				break;
			default:
				break;
			}
		}

		case CEvent::KEYBOARD: {
			auto& keyboard_handler = KeyboardHandler::GetInstance();
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
		g_logger.Log(Logger::ERROR, "Expected access error: " + std::string(ObjectErrorToString(error.error())));
	}
}
