#pragma once
#include "EventListener.h"

class CEventListenerAtspi final : public IEventListener {
public:

	explicit CEventListenerAtspi();
	~CEventListenerAtspi();

	void Post(IEvent& event) override;

	[[nodiscard]] EventQueue RequestQueue() override;
};

