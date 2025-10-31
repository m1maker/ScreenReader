#pragma once
#include "Event.h"
#include <memory>
#include <vector>

using EventQueue = std::shared_ptr<std::vector<CEvent>>;

class IEventListener
protected:
	EventQueue m_eventQueue;
public:

	IEventListener() = default;
	virtual ~IEventListener() = default;

	virtual void Post(CEvent& event) = 0;

	[[nodiscard]] virtual EventQueue RequestQueue() = 0;
};

