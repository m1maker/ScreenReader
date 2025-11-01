#include "EventListenerAtspi.h"

void CEventListenerAtspi::OnEventCallback(AtspiEvent* event, void* user_data) {
}

CEventListenerAtspi::CEventListenerAtspi() : m_listener(atspi_event_listener_new(&CEventListenerAtspi::OnEventCallback, nullptr, nullptr)) {
	atspi_event_listener_register (&*m_listener, "object:state-changed:focused", nullptr);
	atspi_event_listener_register (&*m_listener, "object:state-changed:selected", nullptr);
}

void CEventListenerAtspi::Post(IEvent& event) {
}

[[nodiscard]] EventQueue CEventListenerAtspi::RequestQueue() {
	return m_eventQueue;
}

