module;
#include <variant>
export module Core.SpeechSystem;
import Core.Environment;
import Traits.SpeechEngine;

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

	[[nodiscard]] inline auto SpeakIfHasParameter(unsigned long long parameter, auto&& on_success, auto&& on_fail)
		-> CSpeechSystem& {
		WithEngine([&](const auto& engine) {
			auto info = engine.GetInfo();
			if (info.supported_parameters & SpeechEngineParameter::SSML) {
				engine.SetParameter(SpeechEngineParameter::SSML, true);
				engine.Speak(on_success());
			}
			else
				engine.Speak(on_fail());
		});
	}
};
