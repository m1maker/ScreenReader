#include "EventHandler.h"
#include "EventListenerAtspi.h"

CEventHandler::CEventHandler() {
	m_listener = std::make_unique<CEventListenerAtspi>();
}

void CEventHandler::Handle() {
	if (!m_listener) [[unlikely]] return;

	auto event_queue = m_listener->RequestQueue();
	if (event_queue.empty()) [[unlikely]] return;

	IEvent event = event_queue.front();
	event_queue.erase(event_queue.begin());

}
