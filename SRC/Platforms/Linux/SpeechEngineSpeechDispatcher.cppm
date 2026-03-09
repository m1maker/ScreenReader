module;
#include <Core/StaticInterface.h>
#include <bitset>
#include <cstdint>
#include <expected>
#include <speech-dispatcher/libspeechd.h>
#include <string_view>
export module Platforms.Linux.SpeechEngine;
import Traits.SpeechEngine;

export class CSpeechEngineSpeechDispatcher final : public TSpeechEngine<CSpeechEngineSpeechDispatcher> {
	::SPDConnection* m_connection{nullptr};
	bool m_enableSpelling{false};
	bool m_ssml{false};

	mutable ::SPDVoice** m_voiceList{nullptr};
	mutable int m_voiceCount{0};
	mutable int m_voiceIndex{0};
	[[nodiscard]] auto SetVoiceIndex() -> int;
	inline void ClearVoiceList() const {
		if (m_voiceList) {
			free_spd_voices(m_voiceList);
			m_voiceList = nullptr;
		}
		m_voiceCount = 0;
	}

	inline void RefreshVoiceList() const {
		ClearVoiceList();
		if (!m_connection) [[unlikely]]
			return;
		m_voiceList = spd_list_synthesis_voices(m_connection);
		if (!m_voiceList)
			return;
		for (; m_voiceList[m_voiceCount] != nullptr; ++m_voiceCount)
			;
	}

	static void SpeechNotificationCallback(size_t message_id, size_t client_id, SPDNotificationType notification_type);

public:
	explicit CSpeechEngineSpeechDispatcher();
	~CSpeechEngineSpeechDispatcher();

	[[nodiscard]] auto do_Test() const -> SpeechEngineResult<> {
		if (m_connection == nullptr) [[unlikely]] {
			return std::unexpected(ESpeechEngineError::DEFUNCT);
		}

		return SpeechEngineResult<>();
	}

	[[nodiscard]] auto do_GetInfo() const -> SpeechEngineResult<SSpeechEngineInfo>;

	auto do_Speak(std::string_view message) -> SpeechEngineResult<SpeechMessage>;
	void do_Stop();
	void do_Cancel();

	void do_Pause(bool pause = true);

	template <typename T> auto do_SetParameter(unsigned long long parameter, T value) -> SpeechEngineResult<>;
	template <typename T>
	[[nodiscard]] auto do_GetParameter(unsigned long long parameter) const -> SpeechEngineResult<T>;

	[[nodiscard]] auto do_GetVoiceInfo(unsigned long long index) -> SpeechEngineResult<SVoiceInfo>;
};

template <typename T>
auto CSpeechEngineSpeechDispatcher::do_SetParameter(unsigned long long parameter, T value) -> SpeechEngineResult<> {
	if (!m_connection) [[unlikely]]
		return std::unexpected(ESpeechEngineError::DEFUNCT);
	using namespace SpeechEngineParameter;
	switch (parameter) {
	/*		case SRAL_PARAM_SYMBOL_LEVEL:
				spd_set_punctuation(speech, static_cast<SPDPunctuation>(*reinterpret_cast<const int*>(value)));
				break;
	*/
	case RATE:
		spd_set_voice_rate(m_connection, value);
		break;
	case VOLUME:
		spd_set_volume(m_connection, value);
		break;
	case SPELLING:
		m_enableSpelling = value;
		break;
	case SSML:
		if (value == m_ssml)
			break;
		spd_set_data_mode(m_connection, value ? SPD_DATA_SSML : SPD_DATA_TEXT);
		m_ssml = value;
		break;
	case VOICE_INDEX: {
		RefreshVoiceList();
		if (!m_voiceList) [[unlikely]]
			return std::unexpected(ESpeechEngineError::FAIL);
		if (spd_set_synthesis_voice(m_connection, m_voiceList[value]->name) == 0) {
			m_voiceIndex = value;
			return SpeechEngineResult<>();
		}
		break;
	}

	[[unlikely]] default:
		return std::unexpected(ESpeechEngineError::INVALID_ARGUMENTS);
	}
	return SpeechEngineResult<>();
}

template <typename T>
[[nodiscard]] auto CSpeechEngineSpeechDispatcher::do_GetParameter(unsigned long long parameter) const
	-> SpeechEngineResult<T> {
	if (!m_connection) [[unlikely]]
		return std::unexpected(ESpeechEngineError::DEFUNCT);
	using namespace SpeechEngineParameter;
	switch (parameter) {
	case RATE:
		return spd_get_voice_rate(m_connection);
	case VOLUME:
		return spd_get_volume(m_connection);
	case SPELLING:
		return m_enableSpelling;
	case SSML:
		return m_ssml;

	case VOICE_COUNT:
		RefreshVoiceList();
		return m_voiceCount;

	case VOICE_INDEX:
		return m_voiceIndex;

	[[unlikely]] default:
		return std::unexpected(ESpeechEngineError::INVALID_ARGUMENTS);
	}
}
