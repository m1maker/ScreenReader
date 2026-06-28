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
#include <atomic>
#include <cstdint>
export module Core.RecursiveObjectIterator;
import Core.Timer;
import Proxies.Object;

enum ERecursiveObjectIteratorInstruction : unsigned char { CONTINUE = 0, BREAK };

class CRecursiveObjectIterator final {
	std::atomic_flag m_shouldStop;
	mutable CTimer m_timer;

public:
	using Callback = ERecursiveObjectIteratorInstruction (*)(CObjectProxy);

private:
	auto Step(CObjectProxy from_start, Callback callback, unsigned char depth, uint64_t timeout_ms) const
		-> ERecursiveObjectIteratorInstruction;

public:
	CRecursiveObjectIterator() = default;

	void Iterate(CObjectProxy from_start, Callback callback, unsigned char depth = 30, uint64_t timeout_ms = 30);

	void Stop() noexcept { m_shouldStop.test_and_set(std::memory_order_acquire); }
};
