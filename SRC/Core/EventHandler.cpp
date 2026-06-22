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
#include <variant>
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
import Core.OutputManager;

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
				if (std::holds_alternative<CObjectEvent>(event.value().operator EventVariant())) {
					auto pool = m_eventQueue.GetPool();
					if (!pool) [[unlikely]] {
						Log(ERROR,
							"Failed to get memory resource from event queue. It is not possible to allocate data to "
							"move the object event for handling to the main thread");
						continue;
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
				}
				else {
					Handle(std::move(event.value()));
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
		std::visit(
			[this](auto&& unpacked_event) {
				using T = std::decay_t<decltype(unpacked_event)>;
				if constexpr (std::is_same_v<T, CObjectEvent>) {
					auto& object_handler = ObjectHandler::GetInstance();
					object_handler.Handle(unpacked_event);
				}
				else if constexpr (std::is_same_v<T, CAnnouncementEvent>) {
					auto& output_manager = OutputManager::GetInstance();
					output_manager.Output(unpacked_event);
				}
				else if constexpr (std::is_same_v<T, CKeyboardEvent>) {
					auto& keyboard_handler = KeyboardHandler::GetInstance();
					keyboard_handler.Handle(unpacked_event);
				}
				else if constexpr (std::is_same_v<T, std::monostate>)
					Log(ERROR, "Failed to dispatch the event because unknown or empty data received");
			},
			event.operator EventVariant());
	}

	catch (const std::exception& standard_exception) {
		LogException(standard_exception);
	}
}
