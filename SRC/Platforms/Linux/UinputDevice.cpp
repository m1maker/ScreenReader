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

module;
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <stdexcept>
#include <string>
#include <thread>
#include <unistd.h>
module Platforms.Linux.UinputDevice;

void CUinputDevice::SetupVirtualDevice() {
	m_uinputFd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

	if (m_uinputFd < 0) {
		for (int i = 0; i < 10; ++i) {
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			m_uinputFd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
			if (m_uinputFd >= 0)
				break;
		}

		if (m_uinputFd < 0) {
			throw std::runtime_error("Failed to open /dev/uinput: " + std::string(strerror(errno)));
		}
	}

	ioctl(m_uinputFd, UI_SET_EVBIT, EV_KEY);
	ioctl(m_uinputFd, UI_SET_EVBIT, EV_SYN);
	ioctl(m_uinputFd, UI_SET_EVBIT, EV_REP);

	for (int i = 0; i < KEY_MAX; ++i) {
		ioctl(m_uinputFd, UI_SET_KEYBIT, i);
	}

	struct uinput_setup usetup{};
	memset(&usetup, 0, sizeof(usetup));
	usetup.id.bustype = BUS_VIRTUAL;
	usetup.id.vendor = 0x1323;
	usetup.id.product = 0x1523;
	strcpy(usetup.name, "MMADESR-Virtual-Keyboard");

	if (ioctl(m_uinputFd, UI_DEV_SETUP, &usetup) < 0)
		throw std::runtime_error("UI_DEV_SETUP failed");
	if (ioctl(m_uinputFd, UI_DEV_CREATE) < 0)
		throw std::runtime_error("UI_DEV_CREATE failed");

	std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

CUinputDevice::CUinputDevice() {
	SetupVirtualDevice();
}

CUinputDevice::~CUinputDevice() {
	if (m_uinputFd >= 0) {
		ioctl(m_uinputFd, UI_DEV_DESTROY);
		close(m_uinputFd);
	}
}

void CUinputDevice::Post(uint16_t type, uint16_t code, int32_t value) {
	if (m_uinputFd < 0)
		return;

	static bool is_first_post{true};
	if (is_first_post) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		is_first_post = false;
	}

	struct input_event ev{};
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

void CUinputDevice::ResetKeys() {
	if (m_uinputFd < 0)
		return;
	struct input_event ev{};
	memset(&ev, 0, sizeof(ev));

	ev.value = 0;
	ev.type = EV_KEY;
	for (int i = 0; i < KEY_MAX; ++i) {
		ev.code = i;
		write(m_uinputFd, &ev, sizeof(ev));
	}
	ev.code = SYN_REPORT;
	write(m_uinputFd, &ev, sizeof(ev));
}