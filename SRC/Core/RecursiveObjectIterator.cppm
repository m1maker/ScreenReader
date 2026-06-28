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

export enum class ERecursiveObjectIteratorInstruction : unsigned char { CONTINUE = 0, BREAK };

export class CRecursiveObjectIterator final {
	std::atomic_flag m_shouldStop;
	mutable CTimer m_timer;

public:
	auto Step(CObjectProxy from_start, auto&& lambda, unsigned char depth, uint64_t timeout_ms) const
		-> ERecursiveObjectIteratorInstruction;

public:
	CRecursiveObjectIterator() = default;

	void Iterate(CObjectProxy from_start, auto&& lambda, unsigned char depth = 30, uint64_t timeout_ms = 30);

	void Stop() noexcept { m_shouldStop.test_and_set(std::memory_order_acquire); }
};

auto CRecursiveObjectIterator::Step(
	CObjectProxy from_start, auto&& lambda, unsigned char depth, uint64_t timeout_ms) const
	-> ERecursiveObjectIteratorInstruction {
	if (depth == 0 || m_shouldStop.test(std::memory_order_acquire))
		return ERecursiveObjectIteratorInstruction::BREAK;

	auto children_count = from_start.GetChildrenCount();
	if (!children_count || *children_count == 0)
		return ERecursiveObjectIteratorInstruction::CONTINUE;

	for (auto i = 0; i < *children_count; ++i) {
		if (m_timer.Elapsed() > timeout_ms || m_shouldStop.test(std::memory_order_acquire))
			return ERecursiveObjectIteratorInstruction::BREAK;
		auto next = from_start.GetChildAt(i);
		if (!next || !next->IsValid())
			continue;
		auto instruction = lambda(*next);
		if (instruction == ERecursiveObjectIteratorInstruction::BREAK)
			return instruction;
		instruction = Step(*next, lambda, depth - 1, timeout_ms);
		if (instruction == ERecursiveObjectIteratorInstruction::BREAK)
			return instruction;
	}
	return ERecursiveObjectIteratorInstruction::CONTINUE;
}

void CRecursiveObjectIterator::Iterate(
	CObjectProxy from_start, auto&& lambda, unsigned char depth, uint64_t timeout_ms) {
	if (!from_start.IsValid()) [[unlikely]]
		return;

	m_shouldStop.clear(std::memory_order::release);
	m_timer.Restart();

	auto children_count = from_start.GetChildrenCount();
	if (!children_count || *children_count == 0)
		return;
	for (auto i = 0; i < *children_count; ++i) {
		if (m_timer.Elapsed() > timeout_ms)
			break;
		auto next = from_start.GetChildAt(i);
		if (!next || !next->IsValid())
			continue;
		auto instruction = Step(*next, lambda, depth, timeout_ms);
		if (instruction == ERecursiveObjectIteratorInstruction::BREAK)
			break;
	}
}
