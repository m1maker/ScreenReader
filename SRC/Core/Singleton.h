#ifndef SINGLETON_H
#define SINGLETON_H

template <class T> class CSingleton {
public:
	static auto GetInstance() -> T& {
		static T instance;
		return instance;
	}

	CSingleton(const CSingleton&) = delete;
	auto operator=(const CSingleton&) -> CSingleton& = delete;
	CSingleton(CSingleton&&) = delete;
	auto operator=(CSingleton&&) -> CSingleton& = delete;

private:
	CSingleton() = default;
	~CSingleton() = default;
};

#define DeclareSingleton(T) friend class CSingleton<T>
#endif
