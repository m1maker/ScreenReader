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
#include <cstddef>
#include <utility>
export module Traits.NonAtomicRefCountedObject;

export template <class Derived, typename Data> class TNonAtomicRefCountedObject {
	mutable struct SControlBlock final {
		unsigned int ref_count;
		Data data;
	}* m_dataBlock{nullptr};

	inline void SetStorage(void* memory) const noexcept { m_dataBlock = static_cast<SControlBlock*>(memory); }

public:
	[[nodiscard]] static consteval auto GetNeededSize() noexcept -> size_t { return sizeof(SControlBlock); }

protected:
	TNonAtomicRefCountedObject() = default;
	explicit TNonAtomicRefCountedObject(void* memory) noexcept {
		SetStorage(memory);
		AddRef();
	}
	explicit TNonAtomicRefCountedObject(const TNonAtomicRefCountedObject& other) noexcept
		: m_dataBlock(other.m_dataBlock) {
		AddRef();
	}
	explicit TNonAtomicRefCountedObject(TNonAtomicRefCountedObject&& other) noexcept
		: m_dataBlock(std::exchange(other.m_dataBlock, nullptr)) {}
	/*virtual*/ ~TNonAtomicRefCountedObject() noexcept { Release(); }

	auto operator=(const TNonAtomicRefCountedObject& other) noexcept -> TNonAtomicRefCountedObject& {
		if (this == &other) [[unlikely]]
			return *this;
		Release();
		m_dataBlock = other.m_dataBlock;
		AddRef();
		return *this;
	}
	auto operator=(TNonAtomicRefCountedObject&& other) noexcept -> TNonAtomicRefCountedObject& {
		if (this == &other) [[unlikely]]
			return *this;
		Release();
		m_dataBlock = std::exchange(other.m_dataBlock, nullptr);
		return *this;
	}

public:
	[[nodiscard]] inline auto GetData(this auto&& self) noexcept -> Data* { return &self.m_dataBlock->data; }
	[[nodiscard]] static constexpr inline auto GetDataAddressFromRawMemory(void* memory) noexcept -> Data* {
		if (!memory) [[unlikely]]
			return nullptr;
		auto control_block = static_cast<SControlBlock*>(memory);
		return &control_block->data;
	}

	inline void AddRef() const noexcept {
		if (!m_dataBlock) [[unlikely]]
			return;
		++m_dataBlock->ref_count;
	}
	inline void Release() noexcept {
		if (!m_dataBlock || m_dataBlock->ref_count == 0) [[unlikely]]
			return;
		else if (--m_dataBlock->ref_count == 0) {
			static_cast<Derived*>(this)->do_OnDestroy();
			m_dataBlock = nullptr;
		}
	}
};
