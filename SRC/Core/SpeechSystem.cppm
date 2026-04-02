module;
#include <cctype>
#include <cstdint>
#include <string_view>
#include <utility>
#include <variant>
export module Core.SpeechSystem;
import Core.App;
import Core.BuiltInSpeechEngine;
import Core.Encoding;
import Core.Logger;
import Core.Speech;
import Core.UnicodeData;

export using SpeechEngineVariant = std::variant<std::monostate, BuiltInSpeechEngine /*, CSpeechEngineRuntime*/>;

export class SpeechSystem final : TModule<"SpeechSystem"> {
	SpeechEngineVariant m_variant;

	explicit SpeechSystem() { m_variant.emplace<BuiltInSpeechEngine>(); }

	~SpeechSystem() = default;

public:
	static auto& GetInstance() {
		static SpeechSystem instance;
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

	[[nodiscard]] inline auto SpeakIfHasParameter(ESpeechEngineParameter parameter, auto&& on_success, auto&& on_fail)
		-> SpeechSystem& {
		WithEngine([&](auto& engine) {
			SSpeechEngineInfo info = engine.GetInfo().value_or({});
			if (info.supported_parameters[std::to_underlying(parameter)]) {
				engine.SetParameter(ESpeechEngineParameter::SSML, true);
				engine.Speak(on_success());
			}
			else
				engine.Speak(on_fail());
		});
		return *this;
	}

	inline auto Speak(std::string_view message,
		bool interrupt = false,
		bool ssml = ScreenReaderApp::GetInstance().GetSettings().speech.ssml) -> SpeechSystem& {
		WithEngine([&](auto& engine) {
			SSpeechEngineInfo info = engine.GetInfo().value_or({});
			if (info.supported_parameters[std::to_underlying(ESpeechEngineParameter::SSML)]) {
				engine.SetParameter(ESpeechEngineParameter::SSML, ssml);
			}

			if (interrupt) {
				engine.Stop();
				engine.Cancel();
			}
			engine.Speak(message);
		});
		return *this;
	}

	inline auto Spell(std::string_view message, bool interrupt = false, bool ssml = false) -> SpeechSystem& {
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

	inline auto Stop() -> SpeechSystem& {
		WithEngine([&](auto& engine) {
			engine.Stop();
			engine.Cancel();
		});
		return *this;
	}
};
