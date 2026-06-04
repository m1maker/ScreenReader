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
#include <utility>
module Core.BuiltInSpeechEngine;
import Core.AudioSystem;
import Core.SpeechSystem;

std::atomic_flag CSpeechEngineEspeakNg::s_stopping{ATOMIC_FLAG_INIT};

template <std::size_t N, typename... Indices> static inline void SetBits(std::bitset<N>& bs, Indices... indices) {
	(bs.set(std::to_underlying(indices)), ...);
}

int CSpeechEngineEspeakNg::SpeakCallback(signed short int* samples, signed int sample_count, espeak_EVENT* events) {
	if (s_stopping.test(std::memory_order_release) || SpeechSystem::GetInstance().ShouldAbort()) {
		return 1;
	}
	if (!samples || !events) {
		return 1;
	}

	AudioDataVector data(samples, samples + sample_count);
	AudioSystem::GetInstance().PushData(0, data);

	return 0;
}

CSpeechEngineEspeakNg::CSpeechEngineEspeakNg() {
	m_initialized = espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, 100, "./espeak-ng-data", 0) > 0;
	if (!m_initialized)
		return;
	espeak_SetSynthCallback(&CSpeechEngineEspeakNg::SpeakCallback);
}

CSpeechEngineEspeakNg::~CSpeechEngineEspeakNg() {
	if (m_initialized)
		espeak_Terminate();
}

[[nodiscard]] auto CSpeechEngineEspeakNg::GetInfo() const -> SpeechEngineResult<SSpeechEngineInfo> {
	SSpeechEngineInfo info;
	info.name = "espeak-ng";
	info.output_mode = ESpeechEngineOutputMode::CALLBACK_FUNCTION;
	using enum ESpeechEngineParameter;
	SetBits(info.supported_parameters, VOLUME, RATE, PITCH, PITCH_RANGE, VOICE_INDEX, VOICE_COUNT, SSML);
	return info;
}

auto CSpeechEngineEspeakNg::Speak(std::string_view message) -> SpeechEngineResult<SpeechMessage> {
	if (!m_initialized) [[unlikely]]
		return std::unexpected(ESpeechEngineError::DEFUNCT);

	s_stopping.clear(std::memory_order_release);
	unsigned int message_id{0};
	auto result =
		espeak_Synth(message.data(), message.length() + 1, 0, POS_CHARACTER, 0, m_flags, &message_id, nullptr);
	if (result == EE_OK)
		return message_id;
	return std::unexpected(ESpeechEngineError::FAIL);
}

void CSpeechEngineEspeakNg::Stop() {
	if (!m_initialized) [[unlikely]]
		return;
	s_stopping.test_and_set(std::memory_order_acquire);
}
