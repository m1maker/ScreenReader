#pragma once
#include "EventListener.h"

class CEventListenerAtspi final
public:

	explicit CEventListenerAtspi();
	~CEventListenerAtspi();

	void Post(IEvent& event);

	[[nodiscard]] EventQueue RequestQueue();
};

