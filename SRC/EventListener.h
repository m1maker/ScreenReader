// Event listener interface
#pragma once
#include "Event.h"
#include <memory>
#include <vector>

/*
A event listener is a platform-specific interface that processes events and converts them to a common screen reader's IEvent.
*/

using EventQueue = std::vector<std::shared_ptr<IEvent>>;

class IEventListener {
protected:
	EventQueue m_eventQueue;
public:

	IEventListener() = default;
	virtual ~IEventListener() = default;

	virtual void Post(std::shared_ptr<IEvent> event) = 0;

	[[nodiscard]] virtual EventQueue& RequestQueue() = 0;
};

