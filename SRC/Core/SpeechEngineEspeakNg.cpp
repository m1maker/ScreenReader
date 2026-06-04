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

std::atomic<bool> CSpeechEngineEspeakNg::s_stopping{false};

template <std::size_t N, typename... Indices> static inline void SetBits(std::bitset<N>& bs, Indices... indices) {
	(bs.set(std::to_underlying(indices)), ...);
}

int CSpeechEngineEspeakNg::SpeakCallback(signed short int* samples, signed int sample_count, espeak_EVENT* events) {
	if (s_stopping.load()) {
		AudioSystem::GetInstance().Stop(0);
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
	m_initialized = espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS, 0, "./espeak-ng-data", 0) > 0;
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
	info.output_mode = ESpeechEngineOutputMode::AUDIO_DEVICE;
	using enum ESpeechEngineParameter;
	SetBits(info.supported_parameters, VOLUME, RATE, PITCH, PITCH_RANGE, VOICE_INDEX, VOICE_COUNT, SSML);
	return info;
}

auto CSpeechEngineEspeakNg::Speak(std::string_view message) -> SpeechEngineResult<SpeechMessage> {
	if (!m_initialized) [[unlikely]]
		return std::unexpected(ESpeechEngineError::DEFUNCT);

	s_stopping.store(false);
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
	s_stopping.store(true);
	AudioSystem::GetInstance().Stop(0);
	espeak_Cancel();
}
