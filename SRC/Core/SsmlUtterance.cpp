module;
#include "ScopedPool.h"

#include <format>
#include <map>
#include <memory>
#include <memory_resource>
#include <regex>
#include <string>
#include <string_view>
module Core.SsmlUtterance;

CSsmlUtterance::CSsmlUtterance()
	: m_currentPitch(cPitchDefault), m_currentRate(cRateDefault), m_currentVolume(cVolumeDefault),
	  m_pool(__buf_, cBufferSize), m_ssmlContent(&m_pool) {
	m_ssmlContent += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	m_ssmlContent += "<speak version=\"1.1\" xmlns=\"http://www.w3.org/2001/10/synthesis\">";
}

void CSsmlUtterance::Text(std::string_view text) {
	if (text.empty()) [[unlikely]]
		return;

	StartProsodyIfNeeded();
	m_ssmlContent += EscapeXml(text);
}

void CSsmlUtterance::Break(std::string_view time) {
	EndProsodyIfNeeded();
	std::format_to(std::back_inserter(m_ssmlContent), "<break time=\"{}\"/>", time);
}

void CSsmlUtterance::Pitch(std::string_view pitch) {
	if (m_currentPitch != pitch) {
		EndProsodyIfNeeded();
		m_currentPitch = pitch;
	}
}

void CSsmlUtterance::Rate(std::string_view rate) {
	if (m_currentRate != rate) {
		EndProsodyIfNeeded();
		m_currentRate = rate;
	}
}

void CSsmlUtterance::Volume(std::string_view volume) {
	if (m_currentVolume != volume) {
		EndProsodyIfNeeded();
		m_currentVolume = volume;
	}
}

void CSsmlUtterance::Voice(std::string_view voice) {
	EndProsodyIfNeeded();
	if (!m_currentVoice.empty()) {
		m_ssmlContent += "</voice>";
	}
	m_currentVoice = voice;
	if (!voice.empty()) [[likely]] {
		std::format_to(std::back_inserter(m_ssmlContent), "<voice name=\"{}\">", EscapeXml(voice));
	}
}

[[nodiscard]] auto CSsmlUtterance::ToSsml() -> std::string_view {
	if (m_inProsody) {
		m_ssmlContent += "</prosody>";
	}

	if (!m_currentVoice.empty()) {
		m_ssmlContent += "</voice>";
	}

	m_ssmlContent += "</speak>";
	return std::string_view(m_ssmlContent);
}

void CSsmlUtterance::Clear() {
	m_ssmlContent.clear();
	m_currentPitch = cPitchDefault;
	m_currentRate = cRateDefault;
	m_currentVolume = cVolumeDefault;
	m_currentVoice = "";
	m_inProsody = false;
	m_ssmlContent += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	m_ssmlContent += "<speak version=\"1.1\" xmlns=\"http://www.w3.org/2001/10/synthesis\">";
}

void CSsmlUtterance::StartProsodyIfNeeded() {
	if (!m_inProsody &&
		(m_currentPitch != cPitchDefault || m_currentRate != cRateDefault || m_currentVolume != cVolumeDefault)) {
		m_ssmlContent += "<prosody";

		if (m_currentPitch != cPitchDefault) {
			std::format_to(std::back_inserter(m_ssmlContent), " pitch=\"{}\"", m_currentPitch);
		}
		if (m_currentRate != cRateDefault) {
			std::format_to(std::back_inserter(m_ssmlContent), " rate=\"{}\"", m_currentRate);
		}
		if (m_currentVolume != cVolumeDefault) {
			std::format_to(std::back_inserter(m_ssmlContent), " volume=\"{}\"", m_currentVolume);
		}

		m_ssmlContent += ">";
		m_inProsody = true;
	}
}

void CSsmlUtterance::EndProsodyIfNeeded() {
	if (m_inProsody) {
		m_ssmlContent += "</prosody>";
		m_inProsody = false;
	}
}

[[nodiscard]] auto CSsmlUtterance::EscapeXml(std::string_view text) -> std::pmr::string {
	std::pmr::string result(&m_pool);

	size_t pos{0};
	while ((pos = result.find('&', pos)) != std::pmr::string::npos) {
		if (result.substr(pos, 5) != "&amp;" && result.substr(pos, 4) != "&lt;" && result.substr(pos, 4) != "&gt;" &&
			result.substr(pos, 6) != "&quot;" && result.substr(pos, 6) != "&apos;") {
			result.replace(pos, 1, "&amp;");
		}
		pos += 1;
	}

	std::regex lt("<");
	std::regex gt(">");
	std::regex quot("\"");
	std::regex apos("'");

	result = std::regex_replace(result, lt, "&lt;");
	result = std::regex_replace(result, gt, "&gt;");
	result = std::regex_replace(result, quot, "&quot;");
	result = std::regex_replace(result, apos, "&apos;");

	return result;
}
