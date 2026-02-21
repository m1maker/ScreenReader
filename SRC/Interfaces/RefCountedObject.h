#pragma once

template <typename T, typename = void> struct SLifecycleTrait final {
	static void AddRef(T*) {}
	static void Release(T*) {}
};
