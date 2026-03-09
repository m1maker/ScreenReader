module;
#include <cctype>
#include <cstdint>
#include <string_view>
#include <variant>
export module Core.SpeechSystem;
import Core.Encoding;
import Core.Environment;
import Core.UnicodeData;
import Traits.SpeechEngine;

export class CSpeechSystem final {
	SpeechEngineVariant m_variant;

	explicit CSpeechSystem() { m_variant.emplace<CDefaultSpeechEngine>(); }

	~CSpeechSystem() = default;

public:
	static auto& GetInstance() {
		static CSpeechSystem instance;
		return instance;
	}

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
		WithEngine([&](auto& engine) {
			SSpeechEngineInfo info = engine.GetInfo().value_or({});
			if (info.supported_parameters & SpeechEngineParameter::SSML) {
				engine.SetParameter(SpeechEngineParameter::SSML, true);
				engine.Speak(on_success());
			}
			else
				engine.Speak(on_fail());
		});
		return *this;
	}

	inline auto Speak(std::string_view message, bool interrupt = true, bool ssml = false) -> CSpeechSystem& {
		WithEngine([&](auto& engine) {
			SSpeechEngineInfo info = engine.GetInfo().value_or({});
			if (info.supported_parameters & SpeechEngineParameter::SSML) {
				engine.SetParameter(SpeechEngineParameter::SSML, ssml);
			}

			if (interrupt) {
				engine.Stop();
				engine.Cancel();
			}
			engine.Speak(message);
		});
		return *this;
	}

	inline auto Spell(std::string_view message, bool interrupt = true, bool ssml = false) -> CSpeechSystem& {
		WithEngine([&](auto& engine) {
			SSpeechEngineInfo info = engine.GetInfo().value_or({});
			/*TODOPITCH			if (info.supported_parameters & SpeechEngineParameter::SSML) {
							engine.SetParameter(SpeechEngineParameter::SSML, ssml);
			}DOTOPITCH*/

			if (interrupt) {
				engine.Stop();
				engine.Cancel();
			}

			CUtf8View message_view(message);

			for (uint32_t c : message_view) {
				if (IsPunctuation(c)) {
					engine.Speak(PunctuationToName(static_cast<char32_t>(c)));
				}

				else {
					auto result = EncodeUtf8(c);
					std::string_view character_data(result.bytes, result.size);
					engine.Speak(character_data);
				}
			}
		});
		return *this;
	}

	inline auto Stop() -> CSpeechSystem& {
		WithEngine([&](auto& engine) {
			engine.Stop();
			engine.Cancel();
		});
		return *this;
	}
};
