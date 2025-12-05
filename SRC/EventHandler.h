// Handling events of different types.
#pragma once
#include "EventListener.h"
#include "Singleton.h"
#include <memory>
#include "Object.h"

class CEventHandler final {
	DeclareSingleton(CEventHandler);
	/*
	A listener is a platform-specific interface that processes events and converts them to a common screen reader's IEvent.
	Then this event handler then queries the queue of these events.
	*/
	std::shared_ptr<IEventListener> m_listener;
	/*
	Let's try to avoid duplicates of the announcer.
	*/
	std::shared_ptr<IObject> m_objectHandledPrevious;
	explicit CEventHandler();
	~CEventHandler() = default;
public:

	void Handle(); // For now, the listener calls it at every new event.

	inline auto GetListener() -> std::shared_ptr<IEventListener> { return m_listener; }
};

#define g_eventHandler CSingleton<CEventHandler>::GetInstance() // Global instance

