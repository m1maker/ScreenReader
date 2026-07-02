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
#include <string_view>
#include <unordered_map>
#include <utility>
export module Core.ObjectCache;
import Core.Environment;
import Core.Object;
import Core.Singleton;

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

	std::pmr::unsynchronized_pool_resource m_pool;
	std::pmr::unordered_map<NativeHandle, SCachedObjectData*> m_cache;

public:
	TObjectCache() : m_cache(&m_pool) {}

	[[nodiscard]] auto GetOrCreate(NativeHandle native_handle) -> PlatformObject {
		if (!native_handle)
			return PlatformObject();

		auto it = m_cache.find(native_handle);
		if (it != m_cache.end()) {
			auto existing_object = PlatformObject(native_handle);
			return existing_object;
		}

		//***...*/		m_cache[native_handle] = object_data;

		/*//		return new_object;*/
	}

	void Remove(NativeHandle native_handle) {
		auto it = m_cache.find(native_handle);
		if (it == m_cache.end()) [[unlikely]]
			return;

		m_cache.erase(it);
	}

	void Clear() {
		for (auto [handle, _] : m_cache) {
			Remove(handle);
		}

		m_cache.clear();
	}
};
