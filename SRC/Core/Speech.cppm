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
	PITCH_RANGE,
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
	VOICE_UNAVAILABLE,
	DATA_UNAVAILABLE,
	TIMEOUT,
	FAIL
};

export [[nodiscard]] constexpr auto SpeechEngineErrorToString(ESpeechEngineError error) -> std::string_view {
	using enum ESpeechEngineError;
	switch (error) {
	case SUCCESS:
		return "No error: The speech operation completed successfully.";
	case DEFUNCT:
		return "Speech Engine Defunct: The speech engine instance is no longer valid or has been destroyed.";
	case NOT_SUPPORTED:
		return "Operation Not Supported: The requested operation is not supported by this speech engine.";
	case ACCESS_DENIED:
		return "Access Denied: Insufficient permissions to access the speech engine or required resources.";
	case INVALID_ARGUMENTS:
		return "Invalid Arguments: One or more parameters provided to the speech method are invalid or out of range.";
	case INVALID_ENCODING:
		return "Invalid Encoding: The specified text encoding format is not supported or malformed.";
	case VOICE_UNAVAILABLE:
		return "Voice Unavailable: The requested voice model is not available, not installed, or failed to load.";
	case DATA_UNAVAILABLE:
		return "Data Unavailable: Required speech data (lexicon, pronunciation dictionary, etc.) is missing or "
			   "inaccessible.";
	case TIMEOUT:
		return "Operation Timeout: The speech operation exceeded the maximum allowed time without completing.";
	case FAIL:
	default:
		return "Unknown Error: An unexpected or unspecified failure occurred during the speech engine operation.";
	}
}

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
