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

export [[nodiscard]] constexpr auto operator+(EUtterancePitchValue pitch) noexcept -> signed char {
	using enum EUtterancePitchValue;
	switch (pitch) {
	case UNKNOWN:
	case DEFAULT:
		return 0;
	case X_LOW:
		return -30;
	case LOW:
		return -15;
	case MEDIUM:
		return -5;
	case HIGH:
		return 25;
	case X_HIGH:
		return 50;
	}
	return 0;
}

export [[nodiscard]] constexpr auto operator+(EUtteranceRateValue rate) noexcept -> signed char {
	using enum EUtteranceRateValue;
	switch (rate) {
	case UNKNOWN:
	case DEFAULT:
		return 0;
	case X_SLOW:
		return -30;
	case SLOW:
		return -15;
	case MEDIUM:
		return -5;
	case FAST:
		return 25;
	case X_FAST:
		return 50;
	}
	return 0;
}

export [[nodiscard]] constexpr auto operator+(EUtteranceVolumeValue volume) noexcept -> signed char {
	using enum EUtteranceVolumeValue;
	switch (volume) {
	case UNKNOWN:
	case SILENT:
	case DEFAULT:
		return 0;
	case X_SOFT:
		return -30;
	case SOFT:
		return -15;
	case MEDIUM:
		return -5;
	case LOUD:
		return 25;
	case X_LOUD:
		return 50;
	}
	return 0;
}

using ValueVariant = std::variant<std::monostate,
	std::string,
	EUtterancePitchValue,
	EUtteranceRateValue,
	EUtteranceVolumeValue,
	unsigned char,
	unsigned long long>;

struct SUtteranceCommand final {
	EUtteranceCommandType type{EUtteranceCommandType::UNKNOWN};
	ValueVariant value;
	bool relative{false};
};

export using UtteranceCommandQueue = std::queue<SUtteranceCommand>;

export class CUtterance final {
	UtteranceCommandQueue m_commands;
	void AddAndEscapeXml(std::string_view text);

public:
	void Begin();
	auto Text(std::string_view text) -> CUtterance&;
	auto Break(unsigned long long time) -> CUtterance&;

	auto Mark(std::string_view name) -> CUtterance&;

	auto Pitch(unsigned char pitch, bool relative = false) -> CUtterance&;
	auto Rate(unsigned char rate, bool relative = false) -> CUtterance&;
	auto Volume(unsigned char volume, bool relative = false) -> CUtterance&;

	auto Voice(std::string_view voice) -> CUtterance&;

	void End();

	[[nodiscard]] auto GetQueue() const noexcept -> UtteranceCommandQueue { return std::move(m_commands); }
};
