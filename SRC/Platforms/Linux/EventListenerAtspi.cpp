// AT-SPI's event listener implementation.
module;
#include <atspi/atspi.h>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <linux/input.h>
#include <sstream>
#include <thread>
#include <unistd.h>
module Platforms.Linux.EventListener;
import Core.AppState;
import Core.Event;
import Core.EventQueue;
import Core.KeyboardHandler;
import Core.Object;
import Core.SpeechSystem;
import Platforms.Linux.Object;
import Platforms.Linux.UinputDevice;
import Proxies.Object;

/*
We won't be tied to AT-SPI device listeners, as it's unreliable. And Evdev will work even on TTY.
*/
[[nodiscard]] auto CEventListenerAtspi::FindKeyboardDevice() -> std::string {
	std::ifstream file("/proc/bus/input/devices");
	if (!file.is_open())
		return "";

	std::string line;
	std::string current_event{""};
	bool is_candidate{false};

	while (std::getline(file, line)) {
		if (line.empty() || line.length() < 2) {
			is_candidate = false;
			current_event = "";
			continue;
		}

		if (line.find("H: Handlers=") != std::string::npos) {
			if (line.find("kbd") != std::string::npos) {
				size_t pos = line.find("event");
				if (pos != std::string::npos) {
					std::stringstream ss(line.substr(pos));
					ss >> current_event;
					is_candidate = true;
				}
			}
		}

		if (is_candidate && line.find("B: KEY=") != std::string::npos) {
			std::string mask = line.substr(line.find('=') + 1);

			if (mask.length() > 30) {
				return "/dev/input/" + current_event;
			}
		}
	}

	return "";
}

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
	auto object =
		CObjectCache<AtspiAccessible, SObjectAtspiData>::GetInstance().GetOrCreate<CObjectAtspi>(event->source);
	object.UpdateCacheByEvent(type);
	object_event.object = CObjectProxy(object);
	EventQueue::GetInstance().Push(std::move(object_event));
}

void CEventListenerAtspi::StartEvdevWatcher() {
	m_keyboardListenerThread = std::jthread([this](const std::stop_token& stop_token) -> void {
		struct input_event ev{};

		while (!stop_token.stop_requested()) {
			auto dev = FindKeyboardDevice();
			if (dev.empty()) {
				std::this_thread::sleep_for(std::chrono::seconds(2));
				continue;
			}

			auto fd = open(dev.c_str(), O_RDONLY | O_NONBLOCK);
			if (fd < 0) {
				std::this_thread::sleep_for(std::chrono::seconds(2));
				continue;
			}

			try {
				CUinputDevice virtual_device(fd);
				unsigned char modifiers{0};
				virtual_device.ResetKeys();
				while (!stop_token.stop_requested()) {
					ssize_t n = read(fd, &ev, sizeof(ev));

					if (n < 0) {
						if (errno == EAGAIN || errno == EWOULDBLOCK) {
							std::this_thread::sleep_for(std::chrono::milliseconds(10));
							continue;
						}
						modifiers = 0;
						break;
					}

					if (n == sizeof(ev) && ev.type == EV_KEY) {

						CKeyboardEvent keyboard_event;
						keyboard_event.hotkey.keycode = LinuxKeycodeToKeyboardEventKeycode(ev.code);

						auto modifier = LinuxModifierToKeyboardEventModifier(ev.code);
						if (ev.value == 1)
							modifiers |= modifier;
						else if (ev.value == 0)
							modifiers &= ~modifier;
						keyboard_event.hotkey.modifiers = modifiers;
						if (!KeyboardHandler::GetInstance().IsHooked(keyboard_event.hotkey)) {
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
					else
						virtual_device.Post(ev.type, ev.code, ev.value);
				}
				virtual_device.ResetKeys();
			}
			catch (const std::exception& standard_exception) {
				LogException(standard_exception);
			}
			if (fd > 0)
				close(fd);
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

	CObjectCache<AtspiAccessible, SObjectAtspiData>::GetInstance();
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

	CObjectCache<AtspiAccessible, SObjectAtspiData>::GetInstance().Clear();
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
