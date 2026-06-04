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
#include <string>
#include <string_view>
export module Core.Utterance;

export class CUtterance final {
	std::pmr::string& m_ssmlContent;
	std::string_view m_currentPitch;
	std::string_view m_currentRate;
	std::string_view m_currentVolume;
	std::string_view m_currentVoice;
	bool m_inProsody{false};

	void StartProsodyIfNeeded();
	void EndProsodyIfNeeded();
	void AddAndEscapeXml(std::string_view text);

public:
	explicit CUtterance(std::pmr::string& ssml);

	void Begin();
	auto Text(std::string_view text) -> CUtterance&;
	auto Break(std::string_view time = "500ms") -> CUtterance&;

	auto Mark(std::string_view name) -> CUtterance&;

	auto Pitch(std::string_view pitch) -> CUtterance&;
	auto Rate(std::string_view rate) -> CUtterance&;
	auto Volume(std::string_view volume) -> CUtterance&;

	auto Voice(std::string_view voice) -> CUtterance&;

	void End();

	static constexpr std::string_view cPitchXlow = "x-low";
	static constexpr std::string_view cPitchLow = "low";
	static constexpr std::string_view cPitchMedium = "medium";
	static constexpr std::string_view cPitchHigh = "high";
	static constexpr std::string_view cPitchXhigh = "x-high";
	static constexpr std::string_view cPitchDefault = "default";

	static constexpr std::string_view cRateXslow = "x-slow";
	static constexpr std::string_view cRateSlow = "slow";
	static constexpr std::string_view cRateMedium = "medium";
	static constexpr std::string_view cRateFast = "fast";
	static constexpr std::string_view cRateXfast = "x-fast";
	static constexpr std::string_view cRateDefault = "default";

	static constexpr std::string_view cVolumeSilent = "silent";
	static constexpr std::string_view cVolumeXsoft = "x-soft";
	static constexpr std::string_view cVolumeSoft = "soft";
	static constexpr std::string_view cVolumeMedium = "medium";
	static constexpr std::string_view cVolumeLoud = "loud";
	static constexpr std::string_view cVolumeXloud = "x-loud";
	static constexpr std::string_view cVolumeDefault = "default";
};
