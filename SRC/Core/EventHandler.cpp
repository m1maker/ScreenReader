// Handling events of different types.
#include "Environment.h"
#include "EventHandler.h"
#if SR_LINUX
#include <Platforms/Linux/EventListenerAtspi.h>
#endif
#include "Logger.h"
#include "EventToSpeech.h"
#include "KeyboardHandler.h"
#include "Action.h"
#include "FocusManager.h"
#include "EventQueue.h"

CEventHandler::CEventHandler() {
#if SR_LINUX
	m_listener = std::make_shared<CEventListenerAtspi>();
#endif
	m_listener->ListenDevice(EDeviceType::KEYBOARD);
	bool success{false};
	success = g_keyboardHandler.RegisterAction(SHotkeyInfo::GetAny(), static_cast<uint32_t>(EAction::STOP_SPEECH));
	success = g_keyboardHandler.RegisterAction(CKeyboardEvent::MODIFIER_SCREEN_READER + CKeyboardEvent::MODIFIER_CTRL + CKeyboardEvent::KEYCODE_K, static_cast<uint32_t>(EAction::STOP_KEYBOARD_HOOKS));

	g_eventToSpeech.AnnounceWhereAmI();
}

void CEventHandler::Start() {
	m_thread = std::jthread([this]() {
		while (g_running.load()) {
			auto event = g_eventQueue.Pop();
			if (event && m_listener) {
				auto pool = g_eventQueue.GetPool();
				if (!pool) [[unlikely]] continue;

				auto raw = pool->allocate(sizeof(CEvent));
				auto raw_event = new(raw) CEvent(std::move(event.value()));
				m_listener->PushToMainThread([](void* pData) {
					if (!pData) return;
					auto event_casted = static_cast<CEvent*>(pData);
					g_eventHandler.Handle(std::move(*event_casted));
					auto pool = g_eventQueue.GetPool();
					if (!pool) [[unlikely]] return;

					event_casted->~CEvent();
					pool->deallocate(pData, sizeof(CEvent));
				}, raw_event);
			}
		}
	});
}

/*
This function is called by an event queue when it has been typecast to IEvent::EEventType and dispatched with the desired event type category.
*/
void CEventHandler::Handle(CEvent&& event) {
	LogCalled();

	try {
		switch (event.GetType()) {
			case CEvent::FOCUS_GAINED: {
				auto object_event = event.GetAs<CObjectEvent>();
				if (!object_event.has_value()) break;
				g_focusManager.SetFocus(object_event.value().object);
				g_speechEngine.Stop();
				g_eventToSpeech.AnnounceFocusChange(event);
				break;
			}
			case CEvent::PARENT_UPDATED:
				g_eventToSpeech.AnnounceWhereAmI();
				break;
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
				std::lock_guard<std::mutex> lock(g_keyboardHandler.m_mutex);
				auto keyboard_event = event.GetAs<CKeyboardEvent>();
				if (!keyboard_event.has_value()) break;
				g_keyboardHandler.m_keysDown[keyboard_event.value().hotkey.keycode] = true;
				g_keyboardHandler.m_modifiers = keyboard_event.value().hotkey.modifiers;

				g_keyboardHandler.Handle(keyboard_event.value());
				break;
			}
			case CEvent::KEY_RELEASED: {
				std::lock_guard<std::mutex> lock(g_keyboardHandler.m_mutex);
				auto keyboard_event = event.GetAs<CKeyboardEvent>();
				if (!keyboard_event.has_value()) break;
				g_keyboardHandler.m_keysDown[keyboard_event.value().hotkey.keycode] = false;
				g_keyboardHandler.m_modifiers = keyboard_event.value().hotkey.modifiers;

				break;
			}
			default:
				break;
		}
	}

	catch(const Sral::Exception& speech_exception) {
		g_logger.Log(CLogger::ERROR, speech_exception.what());
	}
	catch (const std::bad_expected_access<unsigned char>& error) {
		g_logger.Log(CLogger::ERROR, "Expected access error: " + std::string(IObject::ErrorToString(error.error())));
	}
}
