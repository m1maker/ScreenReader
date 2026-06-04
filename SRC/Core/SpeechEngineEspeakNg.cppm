module;
#include <atomic>
#include <bitset>
#include <cstdint>
#include <espeak-ng/speak_lib.h>
#include <expected>
#include <string_view>
export module Core.BuiltInSpeechEngine;
import Core.Speech;

template <typename From, typename To>
[[nodiscard]] static auto MapRange(From value, From from_low, From from_high, To to_low, To to_high) -> To {
	return to_low + (value - from_low) * (to_high - to_low) / (from_high - from_low);
}

[[nodiscard]] constexpr auto GetSpeechEngineParameterFromEspeakParameter(espeak_PARAMETER parameter)
	-> ESpeechEngineParameter {
	using enum ESpeechEngineParameter;
	switch (parameter) {
	case espeakRATE:
		return RATE;
	case espeakPITCH:
		return PITCH;
	case espeakRANGE:
		return PITCH_RANGE;
	default:
		return NONE;
	}
}

[[nodiscard]] constexpr auto GetEspeakParameterFromSpeechEngineParameter(ESpeechEngineParameter parameter)
	-> espeak_PARAMETER {
	using enum ESpeechEngineParameter;
	switch (parameter) {
	case RATE:
		return espeakRATE;
	case PITCH:
		return espeakPITCH;
	case PITCH_RANGE:
		return espeakRANGE;
	default:
		return espeakSILENCE;
	}
}

export class CSpeechEngineEspeakNg final {
	static std::atomic<bool> s_stopping;
	bool m_initialized{false};

	unsigned int m_flags{espeakCHARS_UTF8};

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
	if (!m_initialized) [[unlikely]]
		return std::unexpected(ESpeechEngineError::DEFUNCT);

	auto espeak_parameter = GetEspeakParameterFromSpeechEngineParameter(parameter);
	if (espeak_parameter == espeakSILENCE) {
		using enum ESpeechEngineParameter;
		switch (parameter) {
		case SSML:
			if (value)
				m_flags |= espeakSSML;
			else
				m_flags &= ~espeakSSML;
			return SpeechEngineResult<>();
		[[unlikely]] default:
			return std::unexpected(ESpeechEngineError::NOT_SUPPORTED);
		}
	}

	if constexpr (!std::is_same_v<decltype(value), bool>) {
		auto espeak_value = parameter == ESpeechEngineParameter::RATE
			? MapRange(value, cSpeechEngineMinValue, cSpeechEngineMaxValue, 80, 450)
			: value;

		auto _ = espeak_SetParameter(espeak_parameter, espeak_value, 0);
		return SpeechEngineResult<>();
	}
	return std::unexpected(ESpeechEngineError::NOT_SUPPORTED);
}

template <typename T>
[[nodiscard]] auto CSpeechEngineEspeakNg::GetParameter(ESpeechEngineParameter parameter) const
	-> SpeechEngineResult<T> {
	if (!m_initialized) [[unlikely]]
		return std::unexpected(ESpeechEngineError::DEFUNCT);

	auto espeak_parameter = GetEspeakParameterFromSpeechEngineParameter(parameter);
	if (espeak_parameter == espeakSILENCE) {
		using enum ESpeechEngineParameter;
		switch (parameter) {
		case SSML:
			return m_flags & espeakSSML;
		[[unlikely]] default:
			return std::unexpected(ESpeechEngineError::NOT_SUPPORTED);
		}
	}
	auto espeak_value = espeak_GetParameter(espeak_parameter, 1);
	if (parameter == ESpeechEngineParameter::RATE) {
		espeak_value = MapRange(espeak_value, 80, 450, cSpeechEngineMinValue, cSpeechEngineMaxValue);
	}
	return espeak_value;
}

export using BuiltInSpeechEngine = CSpeechEngineEspeakNg;
