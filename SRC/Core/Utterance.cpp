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

void CUtterance::Begin() {}

auto CUtterance::Text(std::string_view text) -> CUtterance& {
	if (text.empty()) [[unlikely]]
		return *this;

	SUtteranceCommand command{.type = EUtteranceCommandType::TEXT, .value = std::string(text)};
	m_commands.push(std::move(command));
	return *this;
}

auto CUtterance::Break(std::string_view time) -> CUtterance& {
	if (time == "0ms" || time.empty())
		return *this;

	SUtteranceCommand command{.type = EUtteranceCommandType::BREAK, .value = std::string(time)};
	m_commands.push(std::move(command));

	return *this;
}

auto CUtterance::Mark(std::string_view name) -> CUtterance& {
	SUtteranceCommand command{.type = EUtteranceCommandType::MARK, .value = std::string(name)};
	m_commands.push(std::move(command));
	return *this;
}

auto CUtterance::Pitch(std::string_view pitch) -> CUtterance& {
	SUtteranceCommand command{.type = EUtteranceCommandType::PITCH, .value = std::string(pitch)};
	m_commands.push(std::move(command));
	return *this;
}

auto CUtterance::Rate(std::string_view rate) -> CUtterance& {
	SUtteranceCommand command{.type = EUtteranceCommandType::RATE, .value = std::string(rate)};
	m_commands.push(std::move(command));

	return *this;
}

auto CUtterance::Volume(std::string_view volume) -> CUtterance& {
	SUtteranceCommand command{.type = EUtteranceCommandType::VOLUME, .value = std::string(volume)};
	m_commands.push(std::move(command));
	return *this;
}

auto CUtterance::Voice(std::string_view voice) -> CUtterance& {
	SUtteranceCommand command{.type = EUtteranceCommandType::VOICE, .value = std::string(voice)};
	m_commands.push(std::move(command));
	return *this;
}

void CUtterance::End() {}
