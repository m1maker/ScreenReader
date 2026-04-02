module;
#include <bitset>
#include <cstdint>
#include <expected>
#include <string_view>
#include <utility>
export module Core.Speech;

export enum class ESpeechEngineParameter : unsigned char {
	NONE = 0,
	RATE,
	VOLUME,
	PITCH,
	SPELLING,
	VOICE_INDEX,
	VOICE_COUNT,
	SSML,
	COUNT
};

export using SpeechEngineParameters = std::bitset<std::to_underlying(ESpeechEngineParameter::COUNT)>;

export enum class ESpeechEngineOutputMode : unsigned char { VOID = 0, AUDIO_DEVICE, PCM_BUFFER };

export enum class ESpeechEngineSyncMode : unsigned char { SYNC = 0, ASYNC };

export constexpr unsigned char cSpeechEngineMinRate = 0;
export constexpr unsigned char cSpeechEngineMaxRate = 255;
export constexpr unsigned char cSpeechEngineMinPitch = 0;
export constexpr unsigned char cSpeechEngineMaxPitch = 255;
export constexpr unsigned char cSpeechEngineMinVolume = 0;
export constexpr unsigned char cSpeechEngineMaxVolume = 255;

export enum class ESpeechEngineError : unsigned char {
	SUCCESS = 0,
	DEFUNCT,
	NOT_SUPPORTED,
	ACCESS_DENIED,
	INVALID_ARGUMENTS,
	INVALID_ENCODING,
	TIMEOUT,
	FAIL
};

export template <typename T = void> using SpeechEngineResult = std::expected<T, ESpeechEngineError>;

export struct SSpeechEngineInfo final {
	std::string_view name;
	ESpeechEngineOutputMode output_mode{ESpeechEngineOutputMode::VOID};
	ESpeechEngineSyncMode sync_mode{ESpeechEngineSyncMode::ASYNC};
	SpeechEngineParameters supported_parameters{};

	constexpr SSpeechEngineInfo() = default;
	~SSpeechEngineInfo() = default;
};

export struct SVoiceInfo final {
	unsigned long long int index{0};
	std::string_view name;
	std::string_view language;
	std::string_view gender;
	std::string_view vendor;
};

export using SpeechMessage = unsigned long long;
