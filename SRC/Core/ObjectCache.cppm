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
#include <string_view>
#include <unordered_map>
#include <utility>
export module Core.ObjectCache;
import Core.Singleton;
import Traits.RefCountedObject;

export template <class PlatformObject> class TObjectCache final : public TSingleton<TObjectCache<PlatformObject>> {
	using NativeHandle = PlatformObject::NativeHandle;
	using ObjectData = PlatformObject::Data;

	std::pmr::unsynchronized_pool_resource m_pool;
	std::pmr::unordered_map<NativeHandle, ObjectData*> m_cache;

public:
	TObjectCache() : m_cache(&m_pool) {}

	[[nodiscard]] auto GetOrCreate(NativeHandle native_handle) -> PlatformObject {
		if (!native_handle)
			return PlatformObject();

		auto it = m_cache.find(native_handle);
		if (it != m_cache.end()) {
			LifecycleTrait<std::remove_pointer_t<decltype(native_handle)>>::Release(native_handle);

			auto existing_object = PlatformObject(native_handle, it->second, &m_pool);
			return existing_object;
		}

		auto raw = m_pool.allocate(sizeof(ObjectData));
		auto object_data = new (raw) ObjectData();

		auto new_object = PlatformObject(native_handle, object_data, &m_pool);
		m_cache[native_handle] = object_data;

		return new_object;
	}

	void Remove(NativeHandle native_handle) {
		auto it = m_cache.find(native_handle);
		if (it == m_cache.end()) [[unlikely]]
			return;

		LifecycleTrait<std::remove_pointer_t<decltype(native_handle)>>::Release(native_handle);
		it->second->~ObjectData();
		m_pool.deallocate(it->second, sizeof(ObjectData));
		m_cache.erase(it);
	}

	void Clear() {
		for (auto [handle, _] : m_cache) {
			Remove(handle);
		}

		m_cache.clear();
	}
};
