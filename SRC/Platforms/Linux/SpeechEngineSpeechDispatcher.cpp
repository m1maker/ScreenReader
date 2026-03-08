module;
#include <Core/EnumUtils.h>
#include <cstdint>
#include <expected>
#include <speech-dispatcher/libspeechd.h>
#include <string>
#include <string_view>
module Platforms.Linux.SpeechEngine;

static constexpr const std::string_view cSpeechDispatcherClientName = "ZhoriqueSR00";

/*
Tell me! How do I get a current voice in SPD?
I couldn't find anything better than choosing the first available voice based on locale.
If someone can do this differently and better, I would be very grateful!
*/
[[nodiscard]] auto CSpeechEngineSpeechDispatcher::SetVoiceIndex() -> int {
	RefreshVoiceList();
	if (!m_voiceList || m_voiceCount == 0) [[unlikely]]
		return 0;

	auto system_locale = setlocale(LC_ALL, "");
	if (!system_locale) [[unlikely]]
		return 0;

	std::string system_lang = system_locale;
	system_lang = system_lang.substr(0, 5);
	auto index = system_lang.find('_');
	if (index != std::string::npos) [[likely]]
		system_lang[index] = '-';
	for (int i = 0; i < m_voiceCount; ++i) {
		if (m_voiceList[i] && m_voiceList[i]->language) [[likely]] {
			std::string voice_lang = m_voiceList[i]->language;
			voice_lang = voice_lang.substr(0, 5);
			if (voice_lang == system_lang) [[likely]] {
				return i;
			}
		}
	}

	return 0;
}

CSpeechEngineSpeechDispatcher::CSpeechEngineSpeechDispatcher() {
	auto address = spd_get_default_address(nullptr);
	if (address == nullptr) [[unlikely]] {
		return;
	}

	m_connection =
		spd_open2(cSpeechDispatcherClientName.data(), nullptr, nullptr, SPD_MODE_THREADED, address, true, nullptr);
	if (m_connection == nullptr) [[unlikely]] {
		return;
	}
	SPDConnectionAddress__free(address);
	address = nullptr;

	/*	m_connection->callback_begin = &CSpeechEngineSpeechDispatcher::SpeechNotificationCallback;
		m_connection->callback_end = &CSpeechEngineSpeechDispatcher::SpeechNotificationCallback;
		m_connection->callback_cancel = &CSpeechEngineSpeechDispatcher::SpeechNotificationCallback;

		spd_set_notification_on(m_connection, SPD_BEGIN);
		spd_set_notification_on(m_connection, SPD_END);
		spd_set_notification_on(m_connection, SPD_CANCEL);
	*/
	auto index = SetVoiceIndex();
	do_SetParameter(SpeechEngineParameter::VOICE_INDEX, index);
}

CSpeechEngineSpeechDispatcher::~CSpeechEngineSpeechDispatcher() {
	ClearVoiceList();
	m_voiceIndex = 0;

	if (m_connection == nullptr) [[unlikely]]
		return;
	spd_close(m_connection);
	m_connection = nullptr;
}

[[nodiscard]] constexpr auto CSpeechEngineSpeechDispatcher::do_GetInfo() const
	-> SpeechEngineResult<SSpeechEngineInfo> {
	static constinit SSpeechEngineInfo info;
	info.name = "SpeechDispatcher";
	info.output_mode = ESpeechEngineOutputMode::AUDIO_DEVICE;

	using namespace SpeechEngineParameter;
	info.supported_parameters |= RATE | VOLUME | PITCH | SPELLING | VOICE_COUNT | VOICE_INDEX | SSML;
	return info;
}

auto CSpeechEngineSpeechDispatcher::do_Speak(std::string_view message) -> SpeechEngineResult<SpeechMessage> {
	if (!m_connection) [[unlikely]]
		return std::unexpected(ESpeechEngineError::DEFUNCT);

	auto result = spd_say(m_connection, SPD_IMPORTANT, message.data());
	if (result < 0)
		return std::unexpected(ESpeechEngineError::FAIL);

	return static_cast<SpeechMessage>(result);
}

void CSpeechEngineSpeechDispatcher::do_Stop() {
	if (!m_connection) [[unlikely]]
		return;

	spd_stop(m_connection);
}

void CSpeechEngineSpeechDispatcher::do_Cancel() {
	if (!m_connection) [[unlikely]]
		return;

	spd_cancel(m_connection);
}

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
