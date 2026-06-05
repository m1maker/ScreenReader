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
#include <format>
#include <memory_resource>
#include <string>
export module Core.Message;
import Core.Config;
import Core.Utterance;

export class CMessage final {
	bool m_ssml;
	std::pmr::memory_resource* m_pool;
	std::pmr::string m_content;
	mutable CUtterance m_utterance;

public:
	explicit CMessage(bool ssml, std::pmr::memory_resource* pool) : m_ssml(ssml), m_pool(pool), m_content(m_pool) {}

	template <typename... Args> void Append(std::format_string<Args...> fmt, Args&&... args) {
		m_utterance.Text(std::format(fmt, std::forward<Args>(args)...));
	}

	void Separate() { Append(" "); }
	inline void ApplyUtteranceParameters(UtteranceParameters parameters) {
		m_utterance.Break(parameters.pause_before)
			.Rate(parameters.rate, true)
			.Pitch(parameters.pitch, true)
			.Volume(parameters.volume, true);
	}

	[[nodiscard]] auto GetBuffer() -> std::pmr::string& { return m_content; }

	[[nodiscard]] operator UtteranceCommandQueue() const { return std::move(m_utterance.GetQueue()); }
};
