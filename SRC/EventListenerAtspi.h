#pragma once
#include "EventListener.h"

class CEventListener final
public:

	IEventListener() = default;
	virtual ~IEventListener() = default;

	virtual void Post(IEvent& event) = 0;

	[[nodiscard]] virtual EventQueue RequestQueue() = 0;
};

