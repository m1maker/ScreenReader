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
#include <atomic>
#include <cstddef>
export module Traits.AtomicRefCountedObject;

export template <class Derived, typename Data> class TAtomicRefCountedObject {
	struct SControlBlock final {
		std::atomic<unsigned int> ref_count;
		Data data;
	};
	mutable std::atomic<SControlBlock*> m_dataBlock{nullptr};

	inline void SetStorage(void* memory) const noexcept {
		m_dataBlock.store(static_cast<SControlBlock*>(memory), std::memory_order_release);
	}

public:
	[[nodiscard]] static consteval auto GetNeededSize() noexcept -> size_t { return sizeof(SControlBlock); }

protected:
	TAtomicRefCountedObject() = default;
	explicit TAtomicRefCountedObject(void* memory) noexcept {
		SetStorage(memory);
		AddRef();
	}
	explicit TAtomicRefCountedObject(const TAtomicRefCountedObject& other) noexcept
		: m_dataBlock(other.m_dataBlock.load(std::memory_order_acquire)) {
		AddRef();
	}
	explicit TAtomicRefCountedObject(TAtomicRefCountedObject&& other) noexcept
		: m_dataBlock(other.m_dataBlock.exchange(nullptr, std::memory_order_acq_rel)) {}
	/*virtual*/ ~TAtomicRefCountedObject() noexcept { Release(); }

	auto operator=(const TAtomicRefCountedObject& other) noexcept -> TAtomicRefCountedObject& {
		if (this == &other) [[unlikely]]
			return *this;
		Release();
		m_dataBlock.store(other.m_dataBlock.load(std::memory_order_acquire), std::memory_order_release);
		AddRef();
		return *this;
	}
	auto operator=(TAtomicRefCountedObject&& other) noexcept -> TAtomicRefCountedObject& {
		if (this == &other) [[unlikely]]
			return *this;
		Release();
		m_dataBlock.store(other.m_dataBlock.exchange(nullptr, std::memory_order_acq_rel), std::memory_order_release);
		return *this;
	}

public:
	[[nodiscard]] inline auto GetData(this auto&& self) noexcept -> Data* {
		return &self.m_dataBlock.load(std::memory_order_acquire)->data;
	}
	[[nodiscard]] static constexpr inline auto GetDataAddressFromRawMemory(void* memory) noexcept -> Data* {
		if (!memory) [[unlikely]]
			return nullptr;
		auto control_block = static_cast<SControlBlock*>(memory);

		return reinterpret_cast<Data*>(reinterpret_cast<size_t>(&control_block->data));
	}

	inline void AddRef() const noexcept {
		auto loaded_block = m_dataBlock.load(std::memory_order_acquire);
		if (!loaded_block) [[unlikely]]
			return;
		loaded_block->ref_count.fetch_add(1, std::memory_order_relaxed);
	}
	inline void Release() noexcept {
		auto loaded_block = m_dataBlock.load(std::memory_order_acquire);
		if (!loaded_block || loaded_block->ref_count.load(std::memory_order::acquire) == 0) [[unlikely]]
			return;
		else if (loaded_block->ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
			static_cast<Derived*>(this)->do_OnDestroy();
			m_dataBlock.store(nullptr);
		}
	}
};
