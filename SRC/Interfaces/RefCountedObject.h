#pragma once

template<class T>
class IRefCountedObject {
protected:
	T* m_instance{nullptr};
public:

	IRefCountedObject(T* instance) : m_instance(instance) {}
	virtual ~IRefCountedObject() = default;

	virtual void AddRef() const = 0;
	virtual void Release() const = 0;

	[[nodiscard]] operator T*() { return m_instance; }
};

