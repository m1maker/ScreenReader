/*
 * This file is part of the Screen Reader project.
 *
 * Copyright (C) 2025-2026  Georgiy Bondarenko (M_maker / m1maker) <georgijbondarenko248@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

module;
#include <array>
#include <format>
#include <map>
#include <memory>
#include <memory_resource>
#include <string>
#include <string_view>
#include <variant>
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
	: m_currentPitch("default"), m_currentRate("default"), m_currentVolume("default"), m_ssmlContent(ssml) {}

void CUtterance::Begin() {
}

auto CUtterance::Text(std::string_view text) -> CUtterance& {
	if (text.empty()) [[unlikely]]
		return *this;

	if (!m_commands.empty()) {
		auto& last_command = m_commands.back();
		if (last_command.type == ECommandType::TEXT) {
			std::visit(
				[text](auto&& value) {
					using T = std::decay_t<decltype(value)>;
					if constexpr (std::is_same_v<T, std::string>) {
						value.append(text);
					}
				},
				last_command.value);
			return *this;
		}
	}

	SCommand command{.type = ECommandType::TEXT, .value = std::string(text)};
	m_commands.push_back(command);
	return *this;
}

auto CUtterance::Break(std::string_view time) -> CUtterance& {
	if (time == "0ms" || time.empty())
		return *this;

	SCommand command{.type = ECommandType::BREAK, .value = std::string(time)};
	m_commands.push_back(command);

	return *this;
}

auto CUtterance::Mark(std::string_view name) -> CUtterance& {
	SCommand command{.type = ECommandType::MARK, .value = std::string(name)};
	m_commands.push_back(command);
	return *this;
}

auto CUtterance::Pitch(std::string_view pitch) -> CUtterance& {
	SCommand command{.type = ECommandType::PITCH, .value = std::string(pitch)};
	m_commands.push_back(command);
	return *this;
}

auto CUtterance::Rate(std::string_view rate) -> CUtterance& {
	SCommand command{.type = ECommandType::RATE, .value = std::string(rate)};
	m_commands.push_back(command);

	return *this;
}

auto CUtterance::Volume(std::string_view volume) -> CUtterance& {
	SCommand command{.type = ECommandType::VOLUME, .value = std::string(volume)};
	m_commands.push_back(command);
	return *this;
}

auto CUtterance::Voice(std::string_view voice) -> CUtterance& {
	SCommand command{.type = ECommandType::VOICE, .value = std::string(voice)};
	m_commands.push_back(command);
	return *this;
}

void CUtterance::End() {
}

void CUtterance::StartProsodyIfNeeded() {
}

void CUtterance::EndProsodyIfNeeded() {
}
