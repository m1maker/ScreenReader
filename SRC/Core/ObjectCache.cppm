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

export template <class PlatformObject> class TObjectCache final : public TSingleton<TObjectCache<PlatformObject>> {
	using NativeHandle = PlatformObject::NativeHandle;
	using ObjectData = PlatformObject::Data;
	struct SCachedObject final {
		ObjectData* data{nullptr};
		CObjectProxy proxy;
		CObjectProxy::CacheType* proxy_cache_memory{nullptr};
	};

	std::pmr::unsynchronized_pool_resource m_pool;
	std::pmr::unordered_map<NativeHandle, SCachedObject> m_cache;

public:
	TObjectCache() : m_cache(&m_pool) {}

	template <typename T = PlatformObject> [[nodiscard]] auto GetOrCreate(NativeHandle native_handle) -> T {
		if (!native_handle)
			return T();

		auto it = m_cache.find(native_handle);
		if (it != m_cache.end()) {
			LifecycleTrait<std::remove_pointer_t<decltype(native_handle)>>::Release(native_handle);

			if constexpr (std::is_same_v<T, PlatformObject>) {
				auto implementation = it->second.proxy.template GetImpl<PlatformObject>();
				return implementation;
			}
			else if constexpr (std::is_same_v<T, CObjectProxy>)
				return it->second.proxy;
		}

		auto raw = m_pool.allocate(sizeof(ObjectData));
		auto object_data = new (raw) ObjectData();

		auto new_object = PlatformObject(native_handle, object_data, &m_pool);
		raw = m_pool.allocate(sizeof(CObjectProxy::CacheType));
		auto proxy_cache_memory = new (raw) CObjectProxy::CacheType;
		SCachedObject cached_object{
			.data = object_data, .proxy = CObjectProxy(new_object), .proxy_cache_memory = proxy_cache_memory};
		cached_object.proxy.SetCacheMemory(proxy_cache_memory);
		m_cache[native_handle] = cached_object;

		if constexpr (std::is_same_v<T, PlatformObject>)
			return new_object;
		else if constexpr (std::is_same_v<T, CObjectProxy>)
			return cached_object.proxy;
	}

	[[nodiscard]] auto GetProxy(PlatformObject platform_object) -> CObjectProxy {
		if (!platform_object.IsValid()) [[unlikely]]
			return CObjectProxy();

		auto native_handle = platform_object.GetNativeHandle();
		if (!native_handle || !*native_handle) [[unlikely]]
			return CObjectProxy();

		auto it = m_cache.find(*native_handle);
		if (it != m_cache.end()) {
			return it->second.proxy;
		}

		return CObjectProxy();
	}

	void Remove(NativeHandle native_handle) {
		auto it = m_cache.find(native_handle);
		if (it == m_cache.end()) [[unlikely]]
			return;

		LifecycleTrait<std::remove_pointer_t<decltype(native_handle)>>::Release(native_handle);
		using T = CObjectProxy::CacheType;
		it->second.proxy_cache_memory->~T();
		m_pool.deallocate(it->second.proxy_cache_memory, sizeof(T));
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
