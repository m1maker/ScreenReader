#include "EventHandler.h"
#include "EventListenerAtspi.h"

CEventHandler::CEventHandler() {
	m_listener = std::make_unique<CEventListenerAtspi>();
}

void CEventHandler::Handle() {
}
