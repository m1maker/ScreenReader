module;
#include <Core/StaticInterface.h>
#include <bitset>
#include <cstdint>
#include <expected>
#include <string_view>
export module Traits.SpeechEngine;

export enum class ESpeechEngineParameter : unsigned char {
	NONE = 0,	 // void
	RATE,		 // unsigned char
	VOLUME,		 // unsigned char
	PITCH,		 // unsigned char
	SPELLING,	 // bool
	VOICE_INDEX, // unsigned long long int
	VOICE_COUNT, // unsigned long long int
	SSML,		 // bool
};

export enum class ESpeechEngineOutputMode : unsigned char { VOID = 0, AUDIO_DEVICE, PCM_BUFFER };

export enum class ESpeechEngineSyncMode : unsigned char { SYNC = 0, ASYNC };

export constexpr const unsigned char cSpeechEngineMinRate = 0;
export constexpr const unsigned char cSpeechEngineMaxRate = 255;
export constexpr const unsigned char cSpeechEngineMinPitch = 0;
export constexpr const unsigned char cSpeechEngineMaxPitch = 255;
export constexpr const unsigned char cSpeechEngineMinVolume = 0;
export constexpr const unsigned char cSpeechEngineMaxVolume = 255;

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
	std::bitset<64> supported_parameters;
};

export struct SVoiceInfo final {
	unsigned long long int index{0};
	std::string_view name;
	std::string_view language;
	std::string_view gender;
	std::string_view vendor;
};

export using SpeechMessage = unsigned long long;

export template <typename Derived> class TSpeechEngine {
	BindStaticInterface(Derived);

public:
	explicit TSpeechEngine() = default;
	~TSpeechEngine() = default;

	[[nodiscard]] auto GetInfo() const -> SpeechEngineResult<SSpeechEngineInfo> { return Impl().do_GetInfo(); }

	[[nodiscard]] auto Speak(std::string_view message) -> SpeechEngineResult<SpeechMessage> {
		return Impl().do_Speak(message);
	}

	void Stop(SpeechMessage message) { Impl().do_Stop(message); }
	void Pause(SpeechMessage message, bool pause = true) { Impl().do_Pause(message, pause); }

	template <typename T>
	[[nodiscard]] auto SetParameter(ESpeechEngineParameter parameter, T value) -> SpeechEngineResult<> {
		return Impl().do_SetParameter(parameter, value);
	}

	template <typename T>
	[[nodiscard]] auto GetParameter(ESpeechEngineParameter parameter) const -> SpeechEngineResult<T> {
		return Impl().do_GetParameter(parameter);
	}

	[[nodiscard]] auto GetVoiceInfo(unsigned long long index) -> SpeechEngineResult<SVoiceInfo> {
		return Impl().do_GetVoiceInfo(index);
	}
};
