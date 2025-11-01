#include "EventListenerAtspi.h"
#include "ObjectAtspi.h"
#include "EventHandler.h"

void CEventListenerAtspi::OnEventCallback(AtspiEvent* event, void* user_data) {
	if (!event || !user_data || !event->type) [[unlikely]] return;

	CEventListenerAtspi* listener = static_cast<CEventListenerAtspi*>(user_data);

	IEvent::EEventType type = GetEventTypeFromString(event->type);
	// Now only Object events.
	CObjectEvent to_post;
	to_post.type = type;
	to_post.object = std::make_shared<CObjectAtspi>(event->source);
	listener->Post(to_post);
}

CEventListenerAtspi::CEventListenerAtspi() : m_listener(atspi_event_listener_new(&CEventListenerAtspi::OnEventCallback, this, nullptr)) {
	atspi_event_listener_register (&*m_listener, "object:state-changed:focused", nullptr);
	atspi_event_listener_register (&*m_listener, "object:state-changed:selected", nullptr);
}

void CEventListenerAtspi::Post(IEvent& event) {
	m_eventQueue.push_back(event);
	g_eventHandler.Handle();
}

[[nodiscard]] EventQueue& CEventListenerAtspi::RequestQueue() {
	return m_eventQueue;
}

