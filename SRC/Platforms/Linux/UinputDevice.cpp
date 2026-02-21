#include "UinputDevice.h"

#include "EventListenerAtspi.h"

#include <chrono>
#include <thread>

void CUinputDevice::SetupVirtualDevice() {
	m_uinputFd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

	if (m_uinputFd < 0) {
		if (CEventListenerAtspi::ElevatePrivileges()) {
			for (int i = 0; i < 10; ++i) {
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				m_uinputFd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
				if (m_uinputFd >= 0)
					break;
			}

			if (m_uinputFd < 0) {
				throw std::runtime_error("Failed to open /dev/uinput after elevation: " + std::string(strerror(errno)));
			}
		}
		else {
			throw std::runtime_error("Privilege elevation failed");
		}
	}

	ioctl(m_uinputFd, UI_SET_EVBIT, EV_KEY);
	ioctl(m_uinputFd, UI_SET_EVBIT, EV_SYN);

	for (int i = 0; i < KEY_MAX; ++i) {
		ioctl(m_uinputFd, UI_SET_KEYBIT, i);
	}

	struct uinput_setup usetup;
	memset(&usetup, 0, sizeof(usetup));
	usetup.id.bustype = BUS_USB;
	usetup.id.vendor = 0x1323;
	usetup.id.product = 0x1523;
	strcpy(usetup.name, "MMADESR-Virtual-Keyboard");

	if (ioctl(m_uinputFd, UI_DEV_SETUP, &usetup) < 0)
		throw std::runtime_error("UI_DEV_SETUP failed");
	if (ioctl(m_uinputFd, UI_DEV_CREATE) < 0)
		throw std::runtime_error("UI_DEV_CREATE failed");
}

CUinputDevice::CUinputDevice(int device_to_grab) : m_devFd(device_to_grab), m_uinputFd(-1) {
	SetupVirtualDevice();
	Grab(m_devFd);
}

void CUinputDevice::Grab(int device_to_grab) {
	if (m_devFd >= 0) {
		ioctl(m_devFd, EVIOCGRAB, 0);
	}

	if (device_to_grab == -1) {
		m_devFd = -1;
		return;
	}
	if (ioctl(device_to_grab, EVIOCGRAB, 1) < 0) {
		throw std::runtime_error("Failed to grab device (EVIOCGRAB)");
	}

	m_devFd = device_to_grab;
}

CUinputDevice::~CUinputDevice() {
	Grab(-1);
	if (m_uinputFd >= 0) {
		ioctl(m_uinputFd, UI_DEV_DESTROY);
		close(m_uinputFd);
	}
}

void CUinputDevice::Post(uint16_t type, uint16_t code, int32_t value) {
	if (m_uinputFd < 0)
		return;

	struct input_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.type = type;
	ev.code = code;
	ev.value = value;

	write(m_uinputFd, &ev, sizeof(ev));

	ev.type = EV_SYN;
	ev.code = SYN_REPORT;
	ev.value = 0;
	write(m_uinputFd, &ev, sizeof(ev));
}
