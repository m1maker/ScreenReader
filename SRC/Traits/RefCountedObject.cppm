export module Traits.RefCountedObject;

export template <typename T, typename = void> struct LifecycleTrait final {
	static void AddRef(T*) {}
	static void Release(T*) {}
};
