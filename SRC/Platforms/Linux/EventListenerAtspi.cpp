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

// AT-SPI's event listener implementation.
module;
#include <atspi/atspi.h>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <linux/input.h>
#include <sstream>
#include <sys/epoll.h>
#include <thread>
#include <unistd.h>
module Platforms.Linux.EventListener;
import Core.AppState;
import Core.Event;
import Core.EventQueue;
import Core.KeyboardHandler;
import Core.Object;
import Core.ObjectCache;
import Core.SpeechSystem;
import Core.Timer;
import Platforms.Linux.DescriptorManager;
import Platforms.Linux.Object;
import Platforms.Linux.UinputDevice;
import Proxies.Object;

/*
AT-SPI has a listener where you need to register the required events one by one and then process them with a callback.
*/
void CEventListenerAtspi::OnObjectEventCallback(AtspiEvent* event, void* user_data) {
	/*
	Since `atspi_event_main` is the main function of `CPlatformDependentWorkerLinux::Loop();`, we must check in the
	event if g_running is false, then we do `atspi_event_quit()`;.
	*/
	if (!g_running.load()) {
		atspi_event_quit();
		return;
	}

	if (!event || !user_data || !event->type) [[unlikely]] {
		return;
	}

	auto type = GetEventTypeFromString(event->type); // The most important thing is to determine the event type.
	if (type == EObjectEventType::NONE) {
		if (strstr(event->type, "announcement") != nullptr) {
			CAnnouncementEvent announcement_event;
			announcement_event.text = g_value_get_string(&event->any_data);
			announcement_event.live = AtspiLiveToObjectLive(static_cast<AtspiLive>(event->detail1));
			EventQueue::GetInstance().Push(std::move(announcement_event));
			// g_value_unset(&event->any_data);
			g_boxed_free(ATSPI_TYPE_EVENT, event);
			return;
		}
		return;
	}

	/*
	I haven't figured out exactly how these detail 1/detail 2 members work yet, but I've figured out which events don't
	need to be dispatched when !detail1 is. This list of types will likely expand once I fully understand what they are.
	*/
	if ((type == EObjectEventType::FOCUS_GAINED) && !event->detail1)
		return;
	CObjectEvent object_event;
	object_event.type = type;
	g_object_ref(event->source);
	auto& cache = TObjectCache<CObjectAtspi>::GetInstance();
	auto object_proxy = cache.GetOrCreate<CObjectProxy>(event->source);
	object_event.object = object_proxy;
	EventQueue::GetInstance().Push(std::move(object_event));
	g_boxed_free(ATSPI_TYPE_EVENT, event);
}

void CEventListenerAtspi::StartEvdevWatcher() {
	m_keyboardListenerThread = std::jthread([this](const std::stop_token& stop_token) -> void {
		try {
			CUinputDevice virtual_device;
			CDescriptorManager descriptor_manager("/dev/input");
			CTimer update_timer;
			virtual_device.ResetKeys();
			while (!stop_token.stop_requested()) {
				if (update_timer.Elapsed() > 500) {
					descriptor_manager.Update();
					update_timer.Restart();
				}

				auto events = descriptor_manager.Poll();
				if (events.empty()) {
					std::this_thread::yield();
					continue;
				}
				for (auto&& event : events) {
					auto fd = event.data.fd;
					struct input_event ev{};
					ssize_t n = read(fd, &ev, sizeof(ev));
					if (errno == EBADFD) {
						descriptor_manager.PushBad(fd);
						break;
					}
					if (n == sizeof(ev)) {
						if (ev.type != EV_KEY) {
							continue;
						}
						CKeyboardEvent keyboard_event;
						keyboard_event.keycode = LinuxKeycodeToKeyboardEventKeycode(ev.code);
						if (!KeyboardHandler::GetInstance().IsHooked(keyboard_event.keycode)) {
							virtual_device.Post(ev.type, ev.code, ev.value);
						}

						switch (ev.value) {
						case 1:
							keyboard_event.type = CKeyboardEvent::KEY_PRESSED;
							break;
						case 0:
							keyboard_event.type = CKeyboardEvent::KEY_RELEASED;
							break;
						default:
							continue;
						}
						EventQueue::GetInstance().Push(std::move(keyboard_event));
					}
				}
			}
		}
		catch (const std::exception& standard_exception) {
			LogException(standard_exception);
		}
	});
}

void CEventListenerAtspi::StopEvdevWatcher() {
	m_keyboardListenerThread.request_stop();
}

void CEventListenerAtspi::do_ListenDevice(EDeviceType device, bool listen) {
	switch (device) {
	case EDeviceType::KEYBOARD:
		if (listen)
			StartEvdevWatcher();
		else
			StopEvdevWatcher();
		break;
	default:
		break;
	}
}

CEventListenerAtspi::CEventListenerAtspi()
	: m_objectEventListener(atspi_event_listener_new(&CEventListenerAtspi::OnObjectEventCallback, this, nullptr)) {
	if (!m_objectEventListener) [[unlikely]] {
		Log(ERROR, "Failed to get event listener");
		return;
	}

	/*
	AT-SPI has a listener where you need to register the required events one by one.
	*/
	GError* error = nullptr;
	for (const auto& [atspi_event_type, _] : cAtspiObjectEventTypeMap) {
		atspi_event_listener_register(m_objectEventListener, atspi_event_type.data(), &error);
		if (error) {
			Log(ERROR, "Failed to register event {}. {}", atspi_event_type.data(), error->message);
			g_error_free(error);
			error = nullptr;
		}
	}

	TObjectCache<CObjectAtspi>::GetInstance();
}

CEventListenerAtspi::~CEventListenerAtspi() {
	if (m_objectEventListener) {
		GError* error = nullptr;
		for (const auto& [atspi_event_type, event_type] : cAtspiObjectEventTypeMap) {
			atspi_event_listener_deregister(m_objectEventListener, atspi_event_type.data(), &error);
			if (error) {
				g_error_free(error);
				error = nullptr;
			}
		}

		g_object_unref(m_objectEventListener);
	}

	TObjectCache<CObjectAtspi>::GetInstance().Clear();
}

struct SInvocationContext final {
	ThreadFunction function;
	void* pUserData;
};

void CEventListenerAtspi::do_PushToMainThread(ThreadFunction function, void* pUserData) {
	if (!function)
		return;

	auto pool = EventQueue::GetInstance().GetPool();
	if (!pool)
		return;
	auto raw = pool->allocate(sizeof(SInvocationContext));
	auto context = new (raw) SInvocationContext{.function = function, .pUserData = pUserData};
	g_main_context_invoke(
		nullptr,
		[](gpointer data) -> gboolean {
			auto casted = static_cast<SInvocationContext*>(data);
			if (!casted)
				return G_SOURCE_REMOVE;
			casted->function(casted->pUserData);

			auto pool = EventQueue::GetInstance().GetPool();
			if (!pool)
				return G_SOURCE_REMOVE;
			casted->~SInvocationContext();
			pool->deallocate(data, sizeof(SInvocationContext));
			return G_SOURCE_REMOVE;
		},
		context);
}
