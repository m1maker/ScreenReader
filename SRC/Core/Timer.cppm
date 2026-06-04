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

// Some inline utilities that make working with STD/other things easier.
module;
#include <chrono>

export module Core.Timer;

export class CTimer final {
	std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;

public:
	explicit CTimer() { Restart(); }

	[[nodiscard]] auto Elapsed() -> uint64_t {
		auto now = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime).count();
	}

	void Restart() { m_startTime = std::chrono::high_resolution_clock::now(); }
};
