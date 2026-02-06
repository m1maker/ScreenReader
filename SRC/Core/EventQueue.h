#pragma once

#include <memory_resource>
#include <deque>
#include <mutex>
#include <condition_variable>
#include "Singleton.h"
#include "EventHandler.h"

class CEventQueue final {
	std::pmr::unsynchronized_pool_resource m_pool;
	std::pmr::deque<CEvent> m_events;

	std::mutex m_mutex;
	std::condition_variable m_cv;
	bool m_stopping{false};
	DeclareSingleton(CEventQueue);
	explicit CEventQueue() : m_events(&m_pool) {}

	~CEventQueue() { Stop(); }
public:

	template<typename... Args>
	void Push(Args&&... args) {
		std::lock_guard lock(m_mutex);
		m_events.emplace_back(std::forward<Args>(args)...);
		m_cv.notify_one();
	}

	auto Pop() -> std::optional<CEvent> {
		std::unique_lock lock(m_mutex);
		m_cv.wait(lock, [this] { return !m_events.empty() || m_stopping; });

		if (m_stopping && m_events.empty()) return std::nullopt;

		CEvent event = std::move(m_events.front());
		m_events.pop_front();

		return event;
	}

	void Stop() {
		{
			std::lock_guard lock(m_mutex);
			m_stopping = true;
		}
		m_cv.notify_all();
	}
};

#define g_eventQueue CSingleton<CEventQueue>::GetInstance()
