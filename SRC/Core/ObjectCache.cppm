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
#include <memory>
#include <memory_resource>
#include <optional>
#include <string.h>
#include <string_view>
#include <unordered_map>
#include <utility>
export module Core.ObjectCache;
import Core.Environment;
import Core.Object;
import Core.Singleton;
import Traits.NonAtomicRefCountedObject;

struct SCachedObjectProperties final {
	std::optional<EObjectType> type;
	std::optional<ObjectStateMask> states;
	std::optional<ObjectCapabilityMask> capabilities;
	std::optional<int> index, cursor;
	std::optional<std::string_view> application_name, name, description;
	std::optional<double> min_value, max_value, current_value;
};

export struct SCachedObjectData final {
	ObjectVariant variant;
	SCachedObjectProperties properties;
};

export template <class PlatformObject> class TObjectCache final : public TSingleton<TObjectCache<PlatformObject>> {
	using NativeHandle = PlatformObject::NativeHandle;
	using RequiredRefCountedObject = TNonAtomicRefCountedObject<void, SCachedObjectData>;

	std::pmr::unsynchronized_pool_resource m_pool;
	std::pmr::unordered_map<NativeHandle, SCachedObjectData*> m_cache;

public:
	TObjectCache() : m_cache(&m_pool) {}

	[[nodiscard]] auto GetOrCreate(NativeHandle native_handle) -> void* {
		if (!native_handle)
			return nullptr;

		auto it = m_cache.find(native_handle);
		if (it != m_cache.end()) {
			return it->second;
		}

		constexpr auto needed_size = RequiredRefCountedObject::GetNeededSize();
		auto raw = m_pool.allocate(needed_size);
		if (!raw) [[unlikely]]
			return nullptr;
		memset(raw, 0, needed_size);
		auto data_start = RequiredRefCountedObject::GetDataAddressFromRawMemory(raw);
		if (!data_start) [[unlikely]]
			return nullptr;
		auto initialized_data = new (data_start) SCachedObjectData();
		if (!initialized_data) [[unlikely]]
			return nullptr;
		initialized_data->variant.emplace<PlatformObject>(native_handle);
		m_cache[native_handle] = raw;
		return raw;
	}

	void Remove(NativeHandle native_handle) {
		auto it = m_cache.find(native_handle);
		if (it == m_cache.end() || !it->second) [[unlikely]]
			return;

		auto initialized_data =
			static_cast<SCachedObjectData*>(RequiredRefCountedObject::GetDataAddressFromRawMemory(it->second));
		if (!initialized_data) [[unlikely]]
			return;
		initialized_data->~SCachedObjectData();
		m_pool.deallocate(it->second, RequiredRefCountedObject::GetNeededSize());
		m_cache.erase(it);
	}

	void Clear() {
		for (auto [handle, _] : m_cache) {
			Remove(handle);
		}

		m_cache.clear();
	}
};
