export module Core.Singleton;

export template <class T> class TSingleton {
public:
	static auto GetInstance() -> T& {
		static T instance;
		return instance;
	}

	TSingleton(const TSingleton&) = delete;
	auto operator=(const TSingleton&) -> TSingleton& = delete;
	TSingleton(TSingleton&&) = delete;
	auto operator=(TSingleton&&) -> TSingleton& = delete;

protected:
	TSingleton() = default;
	~TSingleton() = default;
};
