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
import Proxies.Object;
import Traits.RefCountedObject;

export template <class NativeHandle, typename ObjectData>
class TObjectCache final : public TSingleton<TObjectCache<NativeHandle, ObjectData>> {
	struct SCachedObject final {
		ObjectData* data{nullptr};
		CObjectProxy proxy;
	};

	std::pmr::unsynchronized_pool_resource m_pool;
	std::pmr::unordered_map<NativeHandle*, SCachedObject> m_cache;

public:
	TObjectCache() : m_cache(&m_pool) {}

	template <typename PlatformObject> [[nodiscard]] auto GetOrCreate(NativeHandle* native_handle) -> PlatformObject {
		if (!native_handle)
			return PlatformObject();

		auto it = m_cache.find(native_handle);
		if (it != m_cache.end()) {
			LifecycleTrait<NativeHandle>::Release(native_handle);

			auto implementation = it->second.proxy.template GetImpl<PlatformObject>();
			return implementation;
		}

		auto raw = m_pool.allocate(sizeof(ObjectData));
		auto object_data = new (raw) ObjectData();

		auto new_object = PlatformObject(native_handle, object_data, &m_pool);
		SCachedObject cached_object{.data = object_data, .proxy = CObjectProxy(new_object)};
		m_cache[native_handle] = cached_object;

		return new_object;
	}

	[[nodiscard]] auto GetProxy(NativeHandle* native_handle) -> CObjectProxy& {
		static CObjectProxy s_invalid;
		auto it = m_cache.find(native_handle);
		if (it == m_cache.end()) [[unlikely]]
			return s_invalid;

		return it->second.proxy;
	}

	void Remove(NativeHandle* native_handle) {
		auto it = m_cache.find(native_handle);
		if (it == m_cache.end()) [[unlikely]]
			return;

		LifecycleTrait<NativeHandle>::Release(native_handle);
		it->second.data->~ObjectData();
		m_pool.deallocate(it->second.data, sizeof(ObjectData));
		m_cache.erase(it);
	}

	void Clear() {
		for (auto [handle, _] : m_cache) {
			Remove(handle);
		}

		m_cache.clear();
	}
};
