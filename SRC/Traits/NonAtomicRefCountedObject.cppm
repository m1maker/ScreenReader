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
#include <utility>
export module Traits.NonAtomicRefCountedObject;

export template <class Derived> class TNonAtomicRefCountedObject {
	unsigned int m_refCount;

protected:
	TNonAtomicRefCountedObject() noexcept : m_refCount(1) {}
	explicit TNonAtomicRefCountedObject(const TNonAtomicRefCountedObject&) noexcept { AddRef(); }
	explicit TNonAtomicRefCountedObject(TNonAtomicRefCountedObject&& other) noexcept
		: m_refCount(std::exchange(other.m_refCount, 0)) {}
	virtual ~TNonAtomicRefCountedObject() noexcept { Release(); }

	auto operator=(const TNonAtomicRefCountedObject& other) noexcept -> TNonAtomicRefCountedObject& {
		if (this == &other) [[unlikely]]
			return *this;
		Release();
		m_refCount = other.m_refCount;
		AddRef();
		return *this;
	}
	auto operator=(TNonAtomicRefCountedObject&& other) noexcept -> TNonAtomicRefCountedObject& {
		if (this == &other) [[unlikely]]
			return *this;
		Release();
		m_refCount = std::exchange(other.m_refCount, 0);
		return *this;
	}

public:
	inline void AddRef() noexcept { ++m_refCount; }
	inline void Release() noexcept {
		if (m_refCount == 0) [[unlikely]]
			return;
		else if (--m_refCount == 0) {
			static_cast<Derived*>(this)->do_OnDestroy();
		}
	}

	[[nodiscard]] auto GetRefCount() const noexcept { return m_refCount; }
};
