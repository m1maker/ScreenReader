/*
 * This file is part of the Screen Reader project.
 *
 * Copyright (C) 2025-2026  Georgiy Bondarenko (M_maker / m1maker) <georgijbondarenko248@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
[[nodiscard]] auto MapRange(From value, From from_low, From from_high, To to_low, To to_high) -> To {
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
	static std::atomic_flag s_stopping;
	bool m_initialized{false};

	unsigned int m_flags{espeakCHARS_UTF8};

	static int SpeakCallback(signed short int* samples, signed int sample_count, espeak_EVENT* events);

public:
	explicit CSpeechEngineEspeakNg();
	~CSpeechEngineEspeakNg();

	[[nodiscard]] auto Test() const -> SpeechEngineResult<> { return SpeechEngineResult<>(); }

	[[nodiscard]] auto GetInfo() const -> SpeechEngineResult<SSpeechEngineInfo>;

	auto Speak(std::string_view message) -> SpeechEngineResult<SpeechMessage>;
	auto Stop() -> SpeechEngineResult<>;

	template <typename T> auto SetParameter(ESpeechEngineParameter parameter, T value) -> SpeechEngineResult<>;
	template <typename T>
	[[nodiscard]] auto GetParameter(ESpeechEngineParameter parameter, T& value) -> SpeechEngineResult<>;

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
[[nodiscard]] auto CSpeechEngineEspeakNg::GetParameter(ESpeechEngineParameter parameter, T& value)
	-> SpeechEngineResult<> {
	if (!m_initialized) [[unlikely]]
		return std::unexpected(ESpeechEngineError::DEFUNCT);

	auto espeak_parameter = GetEspeakParameterFromSpeechEngineParameter(parameter);
	if (espeak_parameter == espeakSILENCE) {
		using enum ESpeechEngineParameter;
		switch (parameter) {
		case SSML:
			value = m_flags & espeakSSML;
			break;
		[[unlikely]] default:
			return std::unexpected(ESpeechEngineError::NOT_SUPPORTED);
		}
	}
	auto espeak_value = espeak_GetParameter(espeak_parameter, 1);
	if (parameter == ESpeechEngineParameter::RATE) {
		espeak_value = MapRange(espeak_value, 80, 450, cSpeechEngineMinValue, cSpeechEngineMaxValue);
	}
	value = espeak_value;
	return SpeechEngineResult<>();
}

export using BuiltInSpeechEngine = CSpeechEngineEspeakNg;
