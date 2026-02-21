#pragma once

template <class T> class IRefCountedObject {
protected:
	T* m_instance{nullptr};

public:
	IRefCountedObject(T* instance) : m_instance(instance) {}
	virtual ~IRefCountedObject() = default;

	virtual void AddRef() noexcept = 0;
	virtual void Release() noexcept = 0;

	[[nodiscard]] operator T*() noexcept { return m_instance; }
	[[nodiscard]] explicit operator bool() const noexcept { return m_instance; }
};
