module;
#include <bitset>
#include <cstdint>
#include <expected>
#include <string_view>
#include <utility>
module Core.BuiltInSpeechEngine;

template <std::size_t N, typename... Indices> static inline void SetBits(std::bitset<N>& bs, Indices... indices) {
	(bs.set(std::to_underlying(indices)), ...);
}

CSpeechEngineEspeakNg::CSpeechEngineEspeakNg() {}

CSpeechEngineEspeakNg::~CSpeechEngineEspeakNg() {}

[[nodiscard]] auto CSpeechEngineEspeakNg::GetInfo() const -> SpeechEngineResult<SSpeechEngineInfo> {
	SSpeechEngineInfo info;
	info.name = "espeak-ng";
	info.output_mode = ESpeechEngineOutputMode::AUDIO_DEVICE;
	using enum ESpeechEngineParameter;
	SetBits(info.supported_parameters, VOLUME, RATE, PITCH, VOICE_INDEX, VOICE_COUNT, SSML);
	return info;
}

auto CSpeechEngineEspeakNg::Speak(std::string_view message) -> SpeechEngineResult<SpeechMessage> {
	return static_cast<SpeechMessage>(-1);
}

void CSpeechEngineEspeakNg::Stop() {}

void CSpeechEngineEspeakNg::Cancel() {}
