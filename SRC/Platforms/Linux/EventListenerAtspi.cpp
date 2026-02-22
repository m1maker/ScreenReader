// AT-SPI's event listener implementation.
#include "EventListenerAtspi.h"

#include "UinputDevice.h"

#include <Core/App.h>
#include <Core/AppState.h>
#include <Core/EventHandler.h>
#include <Core/EventQueue.h>
#include <Core/KeyboardHandler.h>
#include <Core/Logger.h>
#include <Core/SpeechEngine.h>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <linux/input.h>
#include <sstream>
#include <thread>
#include <unistd.h>

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

	[[maybe_unused]] CScopedCategory _("ATSPI object event callback");
	if (!event || !user_data || !event->type) [[unlikely]] {
		g_logger.Log(CLogger::DEBUG, "One or more pointers required by event handler was nullptr");
		return;
	}

	auto type = GetEventTypeFromString(event->type); // The most important thing is to determine the event type.
	if (type == CObjectEvent::NONE)
		return;

	/*
	I haven't figured out exactly how these detail 1/detail 2 members work yet, but I've figured out which events don't
	need to be dispatched when !detail1 is. This list of types will likely expand once I fully understand what they are.
	*/
	if ((type == CObjectEvent::FOCUS_GAINED) && !event->detail1)
		return;
	CObjectEvent object_event;
	object_event.type = type;
	g_object_ref(event->source);
	object_event.object = g_objectCache(AtspiAccessible, CObjectAtspi).GetOrCreate(event->source);
	object_event.object->UpdateCacheByEvent(type);
	/*
	Here's the CEvent::now flag. It's currently used to determine whether to interrupt the speaker or wait for their
	turn.
	*/
	g_eventQueue.Push(std::move(object_event), true);
}

void CEventListenerAtspi::StartEvdevWatcher() {
	m_listenKeyboard.store(true);
	m_keyboardListenerThread = std::jthread([this](const std::stop_token& stop_token) -> void {
		LogCalled();
		struct input_event ev{};

		while (!stop_token.stop_requested()) {
			auto dev = FindKeyboardDevice();
			if (dev.empty()) {
				g_logger.Log(CLogger::ERROR, "Keyboard device not found. Retrying in 2s...");
				std::this_thread::sleep_for(std::chrono::seconds(2));
				continue;
			}
			g_logger.Log(CLogger::DEBUG, "Found" + dev);

			auto fd = open(dev.c_str(), O_RDONLY | O_NONBLOCK);

			if (fd == -1) {
				if (errno == EACCES) {
					g_logger.Log(CLogger::INFO, "Access denied to " + dev + ". Requesting privileges...");

					g_speechEngine.Speak("Please authenticate to allow keyboard intercepting.", true);

					if (ElevatePrivileges()) {
						std::this_thread::sleep_for(std::chrono::milliseconds(500));

						fd = open(dev.c_str(), O_RDONLY | O_NONBLOCK);
					}
				}
			}

			if (fd == -1) {
				g_logger.Log(CLogger::ERROR, "Could not open evdev (" + dev + "): " + std::string(strerror(errno)));
				std::this_thread::sleep_for(std::chrono::seconds(2));
				continue;
			}

			g_logger.Log(CLogger::INFO, "Evdev listener started on: " + dev);

			CUinputDevice virtual_device(fd);
			unsigned char modifiers{0};
			while (!stop_token.stop_requested()) {
				ssize_t n = read(fd, &ev, sizeof(ev));

				if (n == -1) {
					if (errno == EAGAIN || errno == EWOULDBLOCK) {
						std::this_thread::sleep_for(std::chrono::milliseconds(10));
						continue;
					}
					g_logger.Log(CLogger::ERROR, "Evdev read error or device disconnected.");
					modifiers = 0;
					break;
				}

				if (n == sizeof(ev) && ev.type == EV_KEY) {
					if (ev.value == 2)
						continue;

					CKeyboardEvent keyboard_event;
					keyboard_event.hotkey.keycode = LinuxKeycodeToKeyboardEventKeycode(ev.code);

					auto modifier = LinuxModifierToKeyboardEventModifier(ev.code);
					if (ev.value == 1)
						modifiers |= modifier;
					else
						modifiers &= ~modifier;
					keyboard_event.hotkey.modifiers = modifiers;
					if (!g_keyboardHandler.IsHooked(keyboard_event.hotkey)) {
						virtual_device.Post(ev.type, ev.code, ev.value);
					}
					keyboard_event.type = (ev.value == 1) ? CKeyboardEvent::KEY_PRESSED : CKeyboardEvent::KEY_RELEASED;
					g_eventQueue.Push(std::move(keyboard_event), false);
				}
				else
					virtual_device.Post(ev.type, ev.code, ev.value);
			}
			close(fd);
			g_logger.Log(CLogger::INFO, "Evdev fd closed.");
		}
	});

	m_keyboardListenerThread.detach();
}

void CEventListenerAtspi::StopEvdevWatcher() {
	m_listenKeyboard.store(false);
	m_keyboardListenerThread.request_stop();
}

void CEventListenerAtspi::do_ListenDevice(EDeviceType device, bool listen) {
	switch (device) {
	case EDeviceType::KEYBOARD:
		listen ? StartEvdevWatcher() : StopEvdevWatcher();
		break;
	default:
		break;
	}
}

CEventListenerAtspi::CEventListenerAtspi()
	: m_objectEventListener(atspi_event_listener_new(&CEventListenerAtspi::OnObjectEventCallback, this, nullptr)) {
	[[maybe_unused]] CScopedCategory _("ATSPI event listener");
	if (!m_objectEventListener) [[unlikely]] {
		g_logger.Log(CLogger::ERROR, "Failed to register the object event listener");
		return;
	}

	/*
	AT-SPI has a listener where you need to register the required events one by one.
	*/
	GError* error = nullptr;
	for (const auto& [atspi_event_type, event_type] : cAtspiObjectEventTypeMap) {
		atspi_event_listener_register(m_objectEventListener, atspi_event_type.data(), &error);
		if (error) {
			g_logger.Log(CLogger::ERROR, "Failed to register event: ", atspi_event_type, error->message);
			g_error_free(error);
			error = nullptr;
		}
	}

	g_objectCache(AtspiAccessible, CObjectAtspi);
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
	StopEvdevWatcher();

	g_objectCache(AtspiAccessible, CObjectAtspi).Clear();
}

[[nodiscard]] auto CEventListenerAtspi::ElevatePrivileges() -> bool {
	static unsigned char counter{0};
	if (counter > 1)
		return false;
	++counter;

	const auto display = std::getenv("DISPLAY");
	const auto wayland_display = std::getenv("WAYLAND_DISPLAY");
	const bool has_gui = (display != nullptr || wayland_display != nullptr);

	const auto user = std::getenv("USER");
	std::string username = user ? user : "root";

	std::string rule =
		"printf '"
		"KERNEL==%bevent*%b, SUBSYSTEM==%binput%b, TAG+=%buaccess%b\\n"
		"KERNEL==%buinput%b, SUBSYSTEM==%bmisc%b, MODE==%b0666%b\\n"
		"' '\"' '\"' '\"' '\"' '\"' '\"' '\"' '\"' '\"' '\"' '\"' '\"' > /etc/udev/rules.d/10-mmadesr.rules";

	std::string final_cmd = std::string(has_gui ? "pkexec" : "sudo") + " sh -c \"" + rule +
		" && setfacl -m u:" + username + ":rw /dev/input/event* " +
		" && udevadm control --reload-rules && udevadm trigger && chmod 666 /dev/uinput\"";

	return std::system(final_cmd.c_str()) == 0;
}

struct SInvocationContext final {
	ThreadFunction function;
	void* pUserData;
};

void CEventListenerAtspi::do_PushToMainThread(ThreadFunction function, void* pUserData) {
	if (!function)
		return;

	auto pool = g_eventQueue.GetPool();
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

			auto pool = g_eventQueue.GetPool();
			if (!pool)
				return G_SOURCE_REMOVE;
			casted->~SInvocationContext();
			pool->deallocate(data, sizeof(SInvocationContext));
			return G_SOURCE_REMOVE;
		},
		context);
}
