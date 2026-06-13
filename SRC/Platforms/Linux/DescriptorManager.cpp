/*
 * This file is part of the Screen Reader project.
 *
 * Copyright (C) 2026  Georgiy Bondarenko (M_maker / m1maker) <georgijbondarenko248@gmail.com>
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
#include <algorithm>
#include <dirent.h>
#include <fcntl.h>
#include <format>
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <string_view>
#include <unistd.h>
#include <vector>

#define NBITS(x) ((((x) - 1) / (sizeof(long int) * 8)) + 1)
#define test_bit(bit, array) (((array)[(bit) / (sizeof(long int) * 8)] >> ((bit) % (sizeof(long) * 8))) & 1)
module Platforms.Linux.DescriptorManager;

[[nodiscard]] auto CDescriptorManager::IsPhysicalKeyboard(int descriptor) noexcept -> bool {
	struct input_id id;
	if (ioctl(descriptor, EVIOCGID, &id) < 0) {
		return false;
	}

	if (id.bustype == BUS_VIRTUAL) {
		return false;
	}

	unsigned long int evbit[NBITS(EV_MAX)];
	if (ioctl(descriptor, EVIOCGBIT(0, sizeof(evbit)), evbit) < 0) {
		return false;
	}

	if (!test_bit(EV_KEY, evbit) || !test_bit(EV_REP, evbit)) {
		return false;
	}

	unsigned long int keybit[NBITS(KEY_MAX)];
	if (ioctl(descriptor, EVIOCGBIT(EV_KEY, sizeof(keybit)), keybit) < 0) {
		return false;
	}

	for (int i = KEY_ESC; i <= KEY_MICMUTE; ++i) {
		if (test_bit(i, keybit)) {
			return true;
		}
	}

	return false;
}

void CDescriptorManager::PushBad(int descriptor) noexcept {
	std::erase(m_descriptors, descriptor);
}

void CDescriptorManager::CloseAll() {
	for (int descriptor : m_descriptors) {
		ioctl(descriptor, EVIOCGRAB, 0);
		close(descriptor);
	}

	m_descriptors.clear();
}

void CDescriptorManager::PushGood(int descriptor) {
	auto it = std::ranges::find(m_descriptors, descriptor);
	if (it != m_descriptors.end())
		return;

	ioctl(descriptor, EVIOCGRAB, 1);
	m_descriptors.push_back(descriptor);
}

void CDescriptorManager::Update() {
	auto directory = opendir(m_currentDirectory.data());
	if (!directory) [[unlikely]]
		return;
	while (auto entry = readdir(directory)) {
		std::string_view name = entry->d_name;
		if (!name.starts_with("event"))
			continue;
		auto path = std::format("{}/{}", m_currentDirectory, name);
		auto descriptor = open(path.c_str(), O_RDONLY | O_NONBLOCK);
		if (descriptor > 0) {
			if (!IsPhysicalKeyboard(descriptor)) {
				close(descriptor);
				continue;
			}
			PushGood(descriptor);
		}
	}
}
