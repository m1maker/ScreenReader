#pragma once
#include "Event.h"
#include <memory>
#include <vector>

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

