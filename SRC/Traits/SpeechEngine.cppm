module;
#include <bitset>
#include <cstdint>
#include <expected>
#include <string_view>
#include <utility>
export module Traits.SpeechEngine;

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

export template <typename Derived> class TSpeechEngine {
public:
	explicit TSpeechEngine() = default;
	~TSpeechEngine() = default;

	[[nodiscard]] auto Test(this auto&& self) -> SpeechEngineResult<> {
		return std::forward<decltype(self)>(self).do_Test();
	}

	[[nodiscard]] auto GetInfo(this auto&& self) -> SpeechEngineResult<SSpeechEngineInfo> {
		return std::forward<decltype(self)>(self).do_GetInfo();
	}

	auto Speak(this auto&& self, std::string_view message) -> SpeechEngineResult<SpeechMessage> {
		if (message.empty())
			return std::unexpected(ESpeechEngineError::INVALID_ARGUMENTS);
		return std::forward<decltype(self)>(self).do_Speak(message);
	}

	void Stop(this auto&& self) { std::forward<decltype(self)>(self).do_Stop(); }
	void Cancel(this auto&& self) { std::forward<decltype(self)>(self).do_Cancel(); }

	void Pause(this auto&& self, bool pause = true) { std::forward<decltype(self)>(self).do_Pause(pause); }

	auto SetParameter(this auto&& self, ESpeechEngineParameter parameter, auto&& value) -> SpeechEngineResult<> {
		return std::forward<decltype(self)>(self).do_SetParameter(parameter, value);
	}

	template <typename Parameter>
	[[nodiscard]] auto GetParameter(this auto&& self, ESpeechEngineParameter parameter)
		-> SpeechEngineResult<Parameter> {
		return std::forward<decltype(self)>(self).do_GetParameter(parameter);
	}

	[[nodiscard]] auto GetVoiceInfo(this auto&& self, unsigned long long index) -> SpeechEngineResult<SVoiceInfo> {
		return std::forward<decltype(self)>(self).do_GetVoiceInfo(index);
	}
};
