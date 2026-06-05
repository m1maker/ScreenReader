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
#include <map>
#include <memory>
#include <memory_resource>
#include <queue>
#include <string>
#include <string_view>
#include <variant>
export module Core.Utterance;

export enum class EUtteranceCommandType : unsigned char { UNKNOWN = 0, TEXT, BREAK, MARK, PITCH, RATE, VOLUME, VOICE };

export enum class EUtterancePitchValue : unsigned char { UNKNOWN = 0, DEFAULT, X_LOW, LOW, MEDIUM, HIGH, X_HIGH };

export enum class EUtteranceRateValue : unsigned char { UNKNOWN = 0, DEFAULT, X_SLOW, SLOW, MEDIUM, FAST, X_FAST };

export enum class EUtteranceVolumeValue : unsigned char {
	UNKNOWN = 0,
	DEFAULT,
	SILENT,
	X_SOFT,
	SOFT,
	MEDIUM,
	LOUD,
	X_LOUD
};

using ValueVariant =
	std::variant<std::monostate, std::string, EUtterancePitchValue, EUtteranceRateValue, EUtteranceVolumeValue, size_t>;

struct SUtteranceCommand final {
	EUtteranceCommandType type{EUtteranceCommandType::UNKNOWN};
	ValueVariant value;
};

export using UtteranceCommandQueue = std::queue<SUtteranceCommand>;

export class CUtterance final {
	UtteranceCommandQueue m_commands;
	void AddAndEscapeXml(std::string_view text);

public:
	void Begin();
	auto Text(std::string_view text) -> CUtterance&;
	auto Break(std::string_view time = "500ms") -> CUtterance&;

	auto Mark(std::string_view name) -> CUtterance&;

	auto Pitch(std::string_view pitch) -> CUtterance&;
	auto Rate(std::string_view rate) -> CUtterance&;
	auto Volume(std::string_view volume) -> CUtterance&;

	auto Voice(std::string_view voice) -> CUtterance&;

	void End();

	[[nodiscard]] auto GetQueue() const noexcept -> UtteranceCommandQueue { return std::move(m_commands); }
};
