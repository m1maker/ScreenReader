#pragma once
#include "Event.h"
#include <memory>
#include <vector>

using EventQueue = std::vector<IEvent>;

class IEventListener {
protected:
	EventQueue m_eventQueue;
public:

	IEventListener() = default;
	virtual ~IEventListener() = default;

	virtual void Post(IEvent& event) = 0;

	[[nodiscard]] virtual EventQueue RequestQueue() = 0;
};

