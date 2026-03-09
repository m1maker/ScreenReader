module;
#include <Core/EnumUtils.h>
#include <Core/Logger.h>
#include <cstdint>
#include <expected>
#include <speech-dispatcher/libspeechd.h>
#include <string.h>
#include <string>
#include <string_view>
#include <unistd.h>
module Platforms.Linux.SpeechEngine;

static constexpr const std::string_view cSpeechDispatcherClientName = "screenreader";

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
	CScopedCategory _("SpeechDispatcher");
	auto address = spd_get_default_address(nullptr);
	if (address == nullptr) [[unlikely]] {
		g_logger.Log(CLogger::ERROR, "Failed to get default address");
		return;
	}

	m_connection =
		spd_open2(cSpeechDispatcherClientName.data(), nullptr, nullptr, SPD_MODE_THREADED, address, true, nullptr);
	if (m_connection == nullptr) [[unlikely]] {
		g_logger.Log(CLogger::ERROR, "Failed to open connection");
		return;
	}

	SPDConnectionAddress__free(address);
	address = nullptr;

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

[[nodiscard]] auto CSpeechEngineSpeechDispatcher::do_GetInfo() const -> SpeechEngineResult<SSpeechEngineInfo> {
	SSpeechEngineInfo info;
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
