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

auto CUtterance::Break(unsigned long long time) -> CUtterance& {
	SUtteranceCommand command{.type = EUtteranceCommandType::BREAK, .value = time};
	m_commands.push(std::move(command));

	return *this;
}

auto CUtterance::Mark(std::string_view name) -> CUtterance& {
	SUtteranceCommand command{.type = EUtteranceCommandType::MARK, .value = std::string(name)};
	m_commands.push(std::move(command));
	return *this;
}

auto CUtterance::Pitch(unsigned char pitch, bool relative) -> CUtterance& {
	SUtteranceCommand command{.type = EUtteranceCommandType::PITCH, .value = pitch, .relative = relative};
	m_commands.push(std::move(command));
	return *this;
}

auto CUtterance::Rate(unsigned char rate, bool relative) -> CUtterance& {
	SUtteranceCommand command{.type = EUtteranceCommandType::RATE, .value = rate, .relative = relative};
	m_commands.push(std::move(command));

	return *this;
}

auto CUtterance::Volume(unsigned char volume, bool relative) -> CUtterance& {
	SUtteranceCommand command{.type = EUtteranceCommandType::VOLUME, .value = volume, .relative = relative};
	m_commands.push(std::move(command));
	return *this;
}

auto CUtterance::Voice(std::string_view voice) -> CUtterance& {
	SUtteranceCommand command{.type = EUtteranceCommandType::VOICE, .value = std::string(voice)};
	m_commands.push(std::move(command));
	return *this;
}

void CUtterance::End() {}
