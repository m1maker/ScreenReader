#pragma once
#include "EventListener.h"
#include <atspi/atspi.h>
#include <memory>

[[nodiscard]] constexpr inline IEvent::EEventType GetEventTypeFromString(gchar* type) {
	if (!type || !*type) [[unlikely]] return IEvent::NONE;

	std::string type_str(type);
	if (type_str == "object:state-changed:focused") return IEvent::FOCUS_GAINED;
	else if (type_str == "object:state-changed:selected") return IEvent::SELECTION_CHANGED;
	return IEvent::NONE;
}

class CEventListenerAtspi final : public IEventListener {
	std::unique_ptr<AtspiEventListener> m_listener;
public:

	static void OnEventCallback(AtspiEvent* event, void* user_data);

	explicit CEventListenerAtspi();
	~CEventListenerAtspi() = default;

	void Post(IEvent& event) override;

	[[nodiscard]] EventQueue& RequestQueue() override;
};

