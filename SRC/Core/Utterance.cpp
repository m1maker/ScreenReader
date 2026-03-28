module;
#include <array>
#include <format>
#include <map>
#include <memory>
#include <memory_resource>
#include <string>
#include <string_view>
module Core.Utterance;

void CUtterance::AddAndEscapeXml(std::string_view text) {
	for (char c : text) {
		switch (c) {
		case '&':
			m_ssmlContent += "&amp;";
			break;
		case '<':
			m_ssmlContent += "&lt;";
			break;
		case '>':
			m_ssmlContent += "&gt;";
			break;
		case '"':
			m_ssmlContent += "&quot;";
			break;
		case '\'':
			m_ssmlContent += "&apos;";
			break;
		default:
			m_ssmlContent += c;
			break;
		}
	}
}

CUtterance::CUtterance(std::pmr::string& ssml)
	: m_currentPitch(cPitchDefault), m_currentRate(cRateDefault), m_currentVolume(cVolumeDefault), m_ssmlContent(ssml) {
}

auto CUtterance::Begin() -> CUtterance& {
	if (m_ssmlContent.size() >= m_prefixLength) {
		Clear(false);
		return *this;
	}

	Clear(true);
	m_ssmlContent += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	m_ssmlContent += "<speak version=\"1.1\" xmlns=\"http://www.w3.org/2001/10/synthesis\">";
	m_prefixLength = m_ssmlContent.size();
	return *this;
}

auto CUtterance::Text(std::string_view text) -> CUtterance& {
	if (text.empty()) [[unlikely]]
		return *this;

	StartProsodyIfNeeded();
	AddAndEscapeXml(text);
	return *this;
}

auto CUtterance::Break(std::string_view time) -> CUtterance& {
	EndProsodyIfNeeded();
	std::format_to(std::back_inserter(m_ssmlContent), "<break time=\"{}\"/>", time);
	return *this;
}

auto CUtterance::Mark(std::string_view name) -> CUtterance& {
	EndProsodyIfNeeded();
	std::format_to(std::back_inserter(m_ssmlContent), "<mark name=\"{}\"/>", name);
	return *this;
}

auto CUtterance::Pitch(std::string_view pitch) -> CUtterance& {
	if (m_currentPitch != pitch) {
		EndProsodyIfNeeded();
		m_currentPitch = pitch;
	}
	return *this;
}

auto CUtterance::Rate(std::string_view rate) -> CUtterance& {
	if (m_currentRate != rate) {
		EndProsodyIfNeeded();
		m_currentRate = rate;
	}
	return *this;
}

auto CUtterance::Volume(std::string_view volume) -> CUtterance& {
	if (m_currentVolume != volume) {
		EndProsodyIfNeeded();
		m_currentVolume = volume;
	}
	return *this;
}

auto CUtterance::Voice(std::string_view voice) -> CUtterance& {
	EndProsodyIfNeeded();
	if (!m_currentVoice.empty()) {
		m_ssmlContent += "</voice>";
	}
	m_currentVoice = voice;
	if (!voice.empty()) [[likely]] {
		std::format_to(std::back_inserter(m_ssmlContent), "<voice name=\"{}\">", voice);
	}
	return *this;
}

void CUtterance::End() {
	if (m_inProsody) {
		m_ssmlContent += "</prosody>";
	}

	if (!m_currentVoice.empty()) {
		m_ssmlContent += "</voice>";
	}

	m_ssmlContent += "</speak>";
}

void CUtterance::Clear(bool all) {
	m_ssmlContent.resize(all ? 0 : m_prefixLength);
	m_currentPitch = cPitchDefault;
	m_currentRate = cRateDefault;
	m_currentVolume = cVolumeDefault;
	m_currentVoice = "";
	m_inProsody = false;
}

void CUtterance::StartProsodyIfNeeded() {
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

void CUtterance::EndProsodyIfNeeded() {
	if (m_inProsody) {
		m_ssmlContent += "</prosody>";
		m_inProsody = false;
	}
}
