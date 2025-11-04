// Handling events of different types.
#pragma once
#include "EventListener.h"
#include "Singleton.h"
#include <memory>

class CEventHandler final {
	/*
	A listener is a platform-specific interface that processes events and converts them to a common screen reader's IEvent.
	Then this event handler then queries the queue of these events.
	*/
	std::shared_ptr<IEventListener> m_listener;
public:
	explicit CEventHandler();
	~CEventHandler() = default;

	void Handle(); // For now, the listener calls it at every new event.

	inline std::shared_ptr<IEventListener> GetListener() { return m_listener; }
};

#define g_eventHandler CSingleton<CEventHandler>::GetInstance() // Global instance

