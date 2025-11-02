#include "EventHandler.h"
#include "EventListenerAtspi.h"
#include "Logger.h"
#include "EventToSpeech.h"

CEventHandler::CEventHandler() {
	m_listener = std::make_unique<CEventListenerAtspi>();
}

void CEventHandler::Handle() {
	[[maybe_unused]] CScopedCatigory _("Event handler");
	if (!m_listener) [[unlikely]] {
		g_logger.Log(CLogger::ERROR, "There is no event listener registered for this platform");
		return;
	}

	auto event_queue = m_listener->RequestQueue();
	if (event_queue.empty()) [[unlikely]] {
		g_logger.Log(CLogger::WARNING, "The event listener was called the handler, but there are no events to handle");
		return;
	}

	for (auto& event : event_queue) {
		switch (event->type) {
			case IEvent::FOCUS_GAINED:
				g_eventToSpeech.AnnounceFocusChange(dynamic_cast<CObjectEvent*>(&*event));
				break;
			default:
				break;
		}
	}

	event_queue.clear();
}
