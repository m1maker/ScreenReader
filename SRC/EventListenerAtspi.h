#pragma once
#include "EventListener.h"
#include <atspi/atspi.h>
#include <memory>

class CEventListenerAtspi final : public IEventListener {
	std::unique_ptr<AtspiEventListener> m_listener;
public:

	static void OnEventCallback(AtspiEvent* event, void* user_data);

	explicit CEventListenerAtspi();
	~CEventListenerAtspi() = default;

	void Post(IEvent& event) override;

	[[nodiscard]] EventQueue RequestQueue() override;
};

