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
#include <cstdint>
#include <span>
#include <string_view>
#include <vector>
export module Platforms.Linux.DescriptorManager;

export class CDescriptorManager final {
	int m_inotifyFd{-1}, m_watchWd{-1}, m_epollFd{-1};

	static constexpr auto cEpollMaxEvents = 128;

	std::vector<int> m_descriptors;

	std::string_view m_currentDirectory;
	void ScanCurrentDirectory();

	void PushGood(int descriptor);

	[[nodiscard]] static auto IsPhysicalKeyboard(int descriptor) noexcept -> bool;

public:
	explicit CDescriptorManager(std::string_view directory);
	~CDescriptorManager() noexcept;

	void PushBad(int descriptor) noexcept;
	void CloseAll();

	void Update();

	[[nodiscard]] auto GetAll() const noexcept -> std::span<const int> { return m_descriptors; }
};
