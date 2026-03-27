module;
#include <Core/EnumUtils.h>
#include <Core/Logger.h>
#include <atomic>
#include <cstdint>
#include <expected>
#include <memory_resource>
#include <speech-dispatcher/libspeechd.h>
#include <stop_token>
#include <string.h>
#include <string>
#include <string_view>
#include <thread>
#include <unistd.h>
module Platforms.Linux.SpeechEngine;

static constexpr const std::string_view cSpeechDispatcherClientName = "screenreader";

/*
Tell me! How do I get a current voice in SPD?
I couldn't find anything better than choosing the first available voice based on locale.
If someone can do this differently and better, I would be very grateful!
*/
void CSpeechEngineSpeechDispatcher::FindVoiceIndex() {
	m_voiceFinder = std::jthread([this](const std::stop_token& stop_token) {
		RefreshVoiceList();
		if (!m_voiceList || m_voiceCount.load() == 0) [[unlikely]]
			return;

		auto system_locale = setlocale(LC_ALL, "");
		if (!system_locale) [[unlikely]]
			return;

		alignas(std::max_align_t) std::array<std::byte, 256> buffer;
		std::pmr::monotonic_buffer_resource pool(&buffer, buffer.size());
		std::pmr::string system_lang(system_locale, &pool);
		system_lang = system_lang.substr(0, 5);
		auto index = system_lang.find('_');
		if (index != std::pmr::string::npos) [[likely]]
			system_lang[index] = '-';
		for (int i = 0; !stop_token.stop_requested() && i < m_voiceCount; ++i) {
			if (m_voiceList[i] && m_voiceList[i]->language) [[likely]] {
				std::pmr::string voice_lang(m_voiceList[i]->language, &pool);
				voice_lang = voice_lang.substr(0, 5);
				if (voice_lang == system_lang) [[likely]] {
					m_voiceIndex.store(i);
					m_voiceFound.store(true);
					return;
				}
			}
		}
	});
	m_voiceFinder.detach();
}

CSpeechEngineSpeechDispatcher::CSpeechEngineSpeechDispatcher() {
	CScopedCategory _("SpeechDispatcher");
	auto address = spd_get_default_address(nullptr);
	if (address == nullptr) [[unlikely]] {
		g_logger.Log(Logger::ERROR, "Failed to get default address");
		return;
	}

	m_connection =
		spd_open2(cSpeechDispatcherClientName.data(), nullptr, nullptr, SPD_MODE_THREADED, address, true, nullptr);
	if (m_connection == nullptr) [[unlikely]] {
		g_logger.Log(Logger::ERROR, "Failed to open connection");
		return;
	}

	SPDConnectionAddress__free(address);
	address = nullptr;
	FindVoiceIndex();
}

CSpeechEngineSpeechDispatcher::~CSpeechEngineSpeechDispatcher() {
	m_voiceFinder.request_stop();
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

	if (m_voiceFound.load()) {
		do_SetParameter(SpeechEngineParameter::VOICE_INDEX, m_voiceIndex.load());
		m_voiceFound.store(false);
	}

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
