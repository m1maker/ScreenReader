module;
#include <bitset>
#include <cstdint>
#include <espeak-ng/speak_lib.h>
#include <expected>
#include <string_view>
#include <utility>
module Core.BuiltInSpeechEngine;
import Core.AudioSystem;

template <std::size_t N, typename... Indices> static inline void SetBits(std::bitset<N>& bs, Indices... indices) {
	(bs.set(std::to_underlying(indices)), ...);
}

int CSpeechEngineEspeakNg::SpeakCallback(signed short int* samples, signed int sample_count, espeak_EVENT* events) {
	if (!samples || !events) [[unlikely]] {
		return 1;
	}

	AudioData data(samples, samples + sample_count);
	AudioSystem::GetInstance().PushData(std::move(data));
	for (espeak_EVENT* event = events; event->type != espeakEVENT_LIST_TERMINATED; ++event) {
		if (event->type == espeakEVENT_MSG_TERMINATED) {
			break;
		}
	}

	return 0;
}

CSpeechEngineEspeakNg::CSpeechEngineEspeakNg() {
	m_initialized = espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 0, "./espeak-ng-data", 0) > 0;
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
	SetBits(info.supported_parameters, VOLUME, RATE, PITCH, VOICE_INDEX, VOICE_COUNT, SSML);
	return info;
}

auto CSpeechEngineEspeakNg::Speak(std::string_view message) -> SpeechEngineResult<SpeechMessage> {
	if (!m_initialized) [[unlikely]]
		return std::unexpected(ESpeechEngineError::DEFUNCT);

	unsigned int message_id{0};
	auto result = espeak_Synth(
		message.data(), message.length() + 1, 0, POS_CHARACTER, 0, espeakCHARS_UTF8 | espeakSSML, &message_id, nullptr);
	if (result == EE_OK)
		return message_id;
	return std::unexpected(ESpeechEngineError::FAIL);
}

void CSpeechEngineEspeakNg::Stop() {
	if (m_initialized)
		espeak_Cancel();
}

void CSpeechEngineEspeakNg::Cancel() {
	Stop();
}
