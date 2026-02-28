// Handling events of different types.
module;
#include "Environment.h"
#if SR_LINUX
#include <Platforms/Linux/EventListenerAtspi.h>
#endif

#include <Traits/Object.h>
#include <memory>
#include <thread>
export module Core.EventHandler;
import Core.EventQueue;
import Core.EventToSpeech;
import Core.FocusManager;

export class CEventHandler final {
	DeclareSingleton(CEventHandler);
	/*
	A listener is a platform-specific trait that processes events and converts them to a common screen reader's
	Event. Then this event handler then queries the queue of these events.
	*/
	CEventListener m_listener;
	CFocusManager& m_focusManager;
	CEventQueue& m_eventQueue;
	CEventToSpeech& m_eventToSpeech;

	std::jthread m_thread;
	explicit CEventHandler();
	~CEventHandler() = default;

public:
	static auto& GetInstance() {
		static CEventHandler instance;
		return instance;
	}

	void Handle(CEvent&& event);

	void Start();

	inline auto GetListener() -> CEventListener* { return &m_listener; }
};
