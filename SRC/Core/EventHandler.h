// Handling events of different types.
#pragma once
#include "Environment.h"
#include "Singleton.h"
#if SR_LINUX
#include <Platforms/Linux/EventListenerAtspi.h>
#endif

#include <Interfaces/Object.h>
#include <memory>
#include <thread>

class CEventHandler final {
	DeclareSingleton(CEventHandler);
	/*
	A listener is a platform-specific trait that processes events and converts them to a common screen reader's
	Event. Then this event handler then queries the queue of these events.
	*/
	CEventListener m_listener;

	std::jthread m_thread;
	explicit CEventHandler();
	~CEventHandler() = default;

public:
	void Handle(CEvent&& event);

	void Start();

	inline auto GetListener() -> CEventListener* { return &m_listener; }
};

#define g_eventHandler CSingleton<CEventHandler>::GetInstance() // Global instance
