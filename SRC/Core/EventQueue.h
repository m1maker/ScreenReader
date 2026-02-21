#pragma once

#include "EventHandler.h"
#include "Singleton.h"

#include <condition_variable>
#include <deque>
#include <memory_resource>
#include <mutex>

class CEventQueue final {
	std::pmr::synchronized_pool_resource m_pool;
	std::pmr::deque<CEvent> m_events;

	std::mutex m_mutex;
	std::condition_variable m_cv;
	bool m_stopping{false};
	DeclareSingleton(CEventQueue);
	explicit CEventQueue() : m_events(&m_pool) {}

	~CEventQueue() { Stop(); }

public:
	template <typename... Args> void Push(Args&&... args) {
		std::scoped_lock lock(m_mutex);
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
			std::scoped_lock lock(m_mutex);
			m_stopping = true;
		}
		m_cv.notify_all();
	}

	[[nodiscard]] auto GetPool() -> std::pmr::synchronized_pool_resource* { return &m_pool; }
};

#define g_eventQueue CSingleton<CEventQueue>::GetInstance()
