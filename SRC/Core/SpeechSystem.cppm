module;
#include <variant>
export module Core.SpeechSystem;
import Core.Environment;

export class CSpeechSystem final {
	SpeechEngineVariant m_variant;

	explicit CSpeechSystem() { SetEngine(CDefaultSpeechEngine()); }

	~CSpeechSystem() = default;

public:
	static auto& GetInstance() {
		static CSpeechSystem instance;
		return instance;
	}

	void SetEngine(auto&& variant) { m_variant = variant; }

	template <typename F> decltype(auto) WithEngine(F&& func) {
		return std::visit(
			[&](auto&& eng) -> decltype(auto) {
				using T = std::decay_t<decltype(eng)>;
				if constexpr (!std::is_same_v<T, std::monostate>) {
					return func(eng);
				}
			},
			m_variant);
	}
};
