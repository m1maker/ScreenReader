// Handling events of different types.
module;
#include "Environment.h"

#include <memory>
#include <thread>
export module Core.EventHandler;
import Core.Event;
import Core.EventQueue;
import Core.EventToSpeech;
import Core.FocusManager;
#if SR_LINUX
import Platforms.Linux.EventListener;
#endif

export class EventHandler final {
	/*
	A listener is a platform-specific trait that processes events and converts them to a common screen reader's
	Event. Then this event handler then queries the queue of these events.
	*/
	CEventListener m_listener;
	CFocusManager& m_focusManager;
	EventQueue& m_eventQueue;
	CEventToSpeech& m_eventToSpeech;

	std::jthread m_thread;
	explicit EventHandler();
	~EventHandler() = default;

public:
	static auto& GetInstance() {
		static EventHandler instance;
		return instance;
	}

	void Handle(CEvent&& event);

	void Start();

	inline auto GetListener() -> CEventListener* { return &m_listener; }
};
