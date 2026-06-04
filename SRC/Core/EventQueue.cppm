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

#include <condition_variable>
#include <deque>
#include <memory_resource>
#include <mutex>
#include <optional>
export module Core.EventQueue;
import Core.Event;
import Core.Logger;
import Core.Singleton;

export class EventQueue final : TModule<"EventQueue">, public TSingleton<EventQueue> {
	std::pmr::synchronized_pool_resource m_pool;
	std::pmr::deque<CEvent> m_events;

	std::mutex m_mutex;
	std::condition_variable m_cv;
	bool m_stopping{false};

public:
	explicit EventQueue() : m_events(&m_pool) {}

	~EventQueue() { Stop(); }

	template <typename... Args> void Push(Args&&... args) {
		std::scoped_lock _(m_mutex);
		m_events.emplace_back(std::forward<Args>(args)...);
		m_cv.notify_one();
	}

	auto Pop() -> std::optional<CEvent> {
		std::unique_lock lock(m_mutex);
		m_cv.wait(lock, [this] -> bool { return !m_events.empty() || m_stopping; });

		if (m_stopping && m_events.empty())
			return std::nullopt;

		CEvent event = std::move(m_events.front());
		m_events.pop_front();

		return event;
	}

	void Stop() {
		{
			std::scoped_lock _(m_mutex);
			m_stopping = true;
		}
		m_cv.notify_all();
	}

	[[nodiscard]] auto GetPool() -> std::pmr::memory_resource* { return &m_pool; }
};
