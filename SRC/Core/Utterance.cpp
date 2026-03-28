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
	m_ssmlContent += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	m_ssmlContent += "<speak version=\"1.1\" xmlns=\"http://www.w3.org/2001/10/synthesis\">";
	m_prefixLength = m_ssmlContent.size();
}

auto CUtterance::Text(this auto&& self, std::string_view text) -> CUtterance& {
	if (self.text.empty()) [[unlikely]]
		return self;

	self.StartProsodyIfNeeded();
	self.AddAndEscapeXml(text);
	return self;
}

auto CUtterance::Break(this auto&& self, std::string_view time) -> CUtterance& {
	self.EndProsodyIfNeeded();
	std::format_to(std::back_inserter(self.m_ssmlContent), "<break time=\"{}\"/>", time);
	return self;
}

auto CUtterance::Mark(this auto&& self, std::string_view name) -> CUtterance& {
	self.EndProsodyIfNeeded();
	std::format_to(std::back_inserter(self.m_ssmlContent), "<mark name=\"{}\"/>", name);
	return self;
}

auto CUtterance::Pitch(this auto&& self, std::string_view pitch) -> CUtterance& {
	if (self.m_currentPitch != pitch) {
		self.EndProsodyIfNeeded();
		self.m_currentPitch = pitch;
	}
	return self;
}

auto CUtterance::Rate(this auto&& self, std::string_view rate) -> CUtterance& {
	if (self.m_currentRate != rate) {
		self.EndProsodyIfNeeded();
		self.m_currentRate = rate;
	}
	return self;
}

auto CUtterance::Volume(this auto&& self, std::string_view volume) -> CUtterance& {
	if (self.m_currentVolume != volume) {
		self.EndProsodyIfNeeded();
		self.m_currentVolume = volume;
	}
	return self;
}

auto CUtterance::Voice(this auto&& self, std::string_view voice) -> CUtterance& {
	self.EndProsodyIfNeeded();
	if (!self.m_currentVoice.empty()) {
		self.m_ssmlContent += "</voice>";
	}
	self.m_currentVoice = voice;
	if (!voice.empty()) [[likely]] {
		std::format_to(std::back_inserter(self.m_ssmlContent), "<voice name=\"{}\">", voice);
	}
	return self;
}

auto CUtterance::Finish(this auto&& self) -> CUtterance& {
	if (self.m_inProsody) {
		self.m_ssmlContent += "</prosody>";
	}

	if (!self.m_currentVoice.empty()) {
		self.m_ssmlContent += "</voice>";
	}

	self.m_ssmlContent += "</speak>";
	return self;
}

void CUtterance::Clear() {
	m_ssmlContent.resize(m_prefixLength);
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
