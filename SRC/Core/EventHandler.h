// Handling events of different types.
#pragma once
#include <Interfaces/EventListener.h>
#include "Singleton.h"
#include <memory>
#include <Interfaces/Object.h>
#include <thread>

class CEventHandler final {
	DeclareSingleton(CEventHandler);
	/*
	A listener is a platform-specific interface that processes events and converts them to a common screen reader's IEvent.
	Then this event handler then queries the queue of these events.
	*/
	std::shared_ptr<IEventListener> m_listener;

	std::thread m_thread;
	explicit CEventHandler();
	~CEventHandler() {
		if (m_thread.joinable()) {
			m_thread.join();
		}
	}
public:

	void Handle(CEvent&& event);

	inline auto GetListener() -> std::shared_ptr<IEventListener> { return m_listener; }
};

#define g_eventHandler CSingleton<CEventHandler>::GetInstance() // Global instance

