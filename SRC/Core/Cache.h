#pragma once

#include <memory_resource>
#include <optional>
#include <type_traits>
#include <utility>

namespace CacheDetails {
template <typename T, typename Value>
void SmartEmplace(std::optional<T>& opt, std::pmr::memory_resource* pool, Value&& val) {
	if constexpr (std::is_constructible_v<T, Value, std::pmr::memory_resource*>) {
		opt.emplace(std::forward<Value>(val), pool);
	}
	else {
		opt.emplace(std::forward<Value>(val));
	}
}
} // namespace CacheDetails

#define DeclareCache(T, name) mutable std::optional<T> m_cache_##name

#define ReturnCache(name)                                                                                              \
	if (m_cache_##name.has_value()) {                                                                                  \
		return m_cache_##name.value();                                                                                 \
	}

#define CacheReturn(name, value_to_cache)                                                                              \
	CacheDetails::SmartEmplace(m_cache_##name, m_pool, value_to_cache);                                                \
	return m_cache_##name.value()

#define ReturnWeakCache(name)                                                                                          \
	if (m_cache_##name.has_value()) {                                                                                  \
		if (auto locked = m_cache_##name.value().lock()) {                                                             \
			return locked;                                                                                             \
		}                                                                                                              \
	}

#define CacheWeakReturn(name, shared_value)                                                                            \
	m_cache_##name.emplace(shared_value);                                                                              \
	return shared_value

#define ReturnCacheTransformed(name, transform_func)                                                                   \
	if (m_cache_##name.has_value()) {                                                                                  \
		return transform_func(m_cache_##name.value());                                                                 \
	}

#define CacheReturnTransformed(name, value_to_cache, transform_func)                                                   \
	CacheDetails::SmartEmplace(m_cache_##name, m_pool, value_to_cache);                                                \
	return transform_func(m_cache_##name.value())

#define Cache(name, value) CacheDetails::SmartEmplace(m_cache_##name, m_pool, value)

#define InvalidateCache(name) m_cache_##name.reset()
