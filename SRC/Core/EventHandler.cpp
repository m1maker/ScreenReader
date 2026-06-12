/*
 * This file is part of the Screen Reader project.
 *
 * Copyright (C) 2025-2026  Georgiy Bondarenko (M_maker / m1maker) <georgijbondarenko248@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

// Handling events of different types.
module;

#include <expected>
#include <mutex>
#include <stop_token>
#include <thread>
#include <utility>
module Core.EventHandler;
import Core.Action;
import Core.App;
import Core.AppState;
import Core.Device;
import Core.Event;
import Core.KeyboardHandler;
import Core.KeyInfo;
import Core.Object;
import Core.ObjectHandler;
import Core.SpeechSystem;

EventHandler::EventHandler() : m_eventQueue(EventQueue::GetInstance()) {
	auto& keyboard_handler = KeyboardHandler::GetInstance();

	bool success{false};
	success = keyboard_handler.RegisterAction(SHotkeyInfo::GetAny(), std::to_underlying(EAction::STOP_SPEECH));
	success = keyboard_handler.RegisterAction(
		MODIFIER_SCREEN_READER + MODIFIER_CTRL + KEYCODE_K, std::to_underlying(EAction::STOP_KEYBOARD_HOOKS), true);
	success = keyboard_handler.RegisterAction(
		MODIFIER_SCREEN_READER + KEYCODE_LEFT, std::to_underlying(EAction::SPIN_ROTOR_LEFT), true);
	success = keyboard_handler.RegisterAction(
		MODIFIER_SCREEN_READER + KEYCODE_RIGHT, std::to_underlying(EAction::SPIN_ROTOR_RIGHT), true);
	success = keyboard_handler.RegisterAction(
		MODIFIER_SCREEN_READER + KEYCODE_UP, std::to_underlying(EAction::ADJUST_ROTOR_UP), true);
	success = keyboard_handler.RegisterAction(
		MODIFIER_SCREEN_READER + KEYCODE_DOWN, std::to_underlying(EAction::ADJUST_ROTOR_DOWN), true);
	success = keyboard_handler.RegisterAction(
		MODIFIER_SCREEN_READER + KEYCODE_ENTER, std::to_underlying(EAction::ADJUST_ROTOR_ACTIVATE), true);

	success = keyboard_handler.RegisterAction(
		MODIFIER_SCREEN_READER + KEYCODE_NUMPAD_8, std::to_underlying(EAction::MOVE_OBJECT_NAVIGATOR_UP), true);
	success = keyboard_handler.RegisterAction(
		MODIFIER_SCREEN_READER + KEYCODE_NUMPAD_2, std::to_underlying(EAction::MOVE_OBJECT_NAVIGATOR_DOWN), true);
	success = keyboard_handler.RegisterAction(
		MODIFIER_SCREEN_READER + KEYCODE_NUMPAD_4, std::to_underlying(EAction::MOVE_OBJECT_NAVIGATOR_LEFT), true);
	success = keyboard_handler.RegisterAction(
		MODIFIER_SCREEN_READER + KEYCODE_NUMPAD_6, std::to_underlying(EAction::MOVE_OBJECT_NAVIGATOR_RIGHT), true);
}

void EventHandler::Start() {
	m_thread = std::jthread([this](const std::stop_token& stop_token) -> void {
		m_listener.ListenDevice(EDeviceType::KEYBOARD);
		while (!stop_token.stop_requested()) {
			auto event = m_eventQueue.Pop();
			if (event) [[likely]] {
				switch (event.value().GetType()) {
				case CEvent::OBJECT: {
					auto pool = m_eventQueue.GetPool();
					if (!pool) [[unlikely]] {
						Log(ERROR,
							"Failed to get memory resource from event queue. It is not possible to allocate data to "
							"move the object event for handling to the main thread");
						break;
					}
					auto raw = pool->allocate(sizeof(CEvent));
					auto raw_event = new (raw) CEvent(std::move(event.value()));
					m_listener.PushToMainThread(
						[](void* pData) -> void {
							if (!pData) [[unlikely]] {
								return;
							}
							auto event_casted = static_cast<CEvent*>(pData);
							EventHandler::GetInstance().Handle(std::move(*event_casted));
							auto pool = EventQueue::GetInstance().GetPool();
							if (!pool) [[unlikely]] {
								return;
							}
							event_casted->~CEvent();
							pool->deallocate(pData, sizeof(CEvent));
						},
						raw_event);
					break;
				}

				case CEvent::KEYBOARD:
					Handle(std::move(event.value()));
					break;
				[[unlikely]] default:
					Log(WARNING, "Unknown event received");
					break;
				}
			}
		}

		m_listener.ListenDevice(EDeviceType::KEYBOARD, false);
	});
}

void EventHandler::Stop() {
	m_thread.request_stop();
	m_eventQueue.Stop();
}

/*
This function is called by an event handler when it has been typecast to IEvent::EEventType by IEventListener and
dispatched with the desired event type category.
*/
void EventHandler::Handle(CEvent&& event) {
	try {
		switch (event.GetType()) {
		case CEvent::OBJECT: {
			auto object_event = event.GetAs<CObjectEvent>();
			if (!object_event.has_value()) {
				Log(WARNING, "An object event received, but it could not be unpacked from the variant");
				break;
			}
			auto& object_handler = ObjectHandler::GetInstance();
			object_handler.Handle(object_event.value());
			break;
		}

		case CEvent::KEYBOARD: {
			auto& keyboard_handler = KeyboardHandler::GetInstance();
			auto keyboard_event = event.GetAs<CKeyboardEvent>();
			if (!keyboard_event.has_value()) {
				Log(WARNING, "A keyboard event received, but it could not be unpacked from the variant");
				break;
			}
			keyboard_handler.Handle(keyboard_event.value());
			break;
		}
		default:
			break;
		}
	}

	catch (const std::exception& standard_exception) {
		LogException(standard_exception);
	}
}
