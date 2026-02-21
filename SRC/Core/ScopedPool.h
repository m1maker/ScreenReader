#pragma once

#include <memory_resource>

#define ScopedPool(name, size)                                                                                         \
	alignas(64) char __buf_##name[size];                                                                               \
	std::pmr::monotonic_buffer_resource name(__buf_##name, size)

#define DefaultPool(name) ScopedPool(name, 16384)
