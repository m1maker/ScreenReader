module;
#include <atomic>
#include <bitset>
#include <cstdint>
#include <espeak-ng/speak_lib.h>
#include <expected>
#include <string_view>
export module Core.BuiltInSpeechEngine;
import Core.Speech;

[[nodiscard]] static constexpr auto GetSpeechEngineParameterFromEspeakParameter(espeak_PARAMETER parameter)
	-> ESpeechEngineParameter {
	using enum ESpeechEngineParameter;
	switch (parameter) {
	case espeakRATE:
		return RATE;
	case espeakPITCH:
		return PITCH;
	default:
		return NONE;
	}
}

export class CSpeechEngineEspeakNg final {
	static std::atomic<bool> s_stopping;
	bool m_initialized{false};

	static int SpeakCallback(signed short int* samples, signed int sample_count, espeak_EVENT* events);

public:
	explicit CSpeechEngineEspeakNg();
	~CSpeechEngineEspeakNg();

	[[nodiscard]] auto Test() const -> SpeechEngineResult<> { return SpeechEngineResult<>(); }

	[[nodiscard]] auto GetInfo() const -> SpeechEngineResult<SSpeechEngineInfo>;

	auto Speak(std::string_view message) -> SpeechEngineResult<SpeechMessage>;
	void Stop();

	template <typename T> auto SetParameter(ESpeechEngineParameter parameter, T value) -> SpeechEngineResult<>;
	template <typename T>
	[[nodiscard]] auto GetParameter(ESpeechEngineParameter parameter) const -> SpeechEngineResult<T>;

	[[nodiscard]] auto GetVoiceInfo(unsigned long long index) const -> SpeechEngineResult<SVoiceInfo>;
};

template <typename T>
auto CSpeechEngineEspeakNg::SetParameter(ESpeechEngineParameter parameter, T value) -> SpeechEngineResult<> {
	return SpeechEngineResult<>();
}

template <typename T>
[[nodiscard]] auto CSpeechEngineEspeakNg::GetParameter(ESpeechEngineParameter parameter) const
	-> SpeechEngineResult<T> {
	return std::unexpected(ESpeechEngineError::FAIL);
}

export using BuiltInSpeechEngine = CSpeechEngineEspeakNg;
