// Handling events of different types.
#include "EventHandler.h"
#include "EventListenerAtspi.h"
#include "Logger.h"
#include "EventToSpeech.h"

CEventHandler::CEventHandler() {
	m_listener = std::make_shared<CEventListenerAtspi>(); // In the future, this will of course be platform specific.
}

/*
This function is called by a platform-specific event listener when it has been typecast to IEvent::EEventType and dispatched with the desired event type category.
*/
void CEventHandler::Handle() {
	[[maybe_unused]] CScopedCategory _("Event handler");
	if (!m_listener) [[unlikely]] {
		g_logger.Log(CLogger::ERROR, "There is no event listener registered for this platform");
		return;
	}

	auto& event_queue = m_listener->RequestQueue();
	if (event_queue.empty()) [[unlikely]] {
		g_logger.Log(CLogger::WARNING, "The event listener was called the handler, but there are no events to handle");
		return;
	}

	for (auto& event : event_queue) {
		switch (event->type) {
			case IEvent::FOCUS_GAINED:
				g_eventToSpeech.AnnounceFocusChange(dynamic_cast<CObjectEvent*>(&*event));
				break;
			case IEvent::VALUE_CHANGED:
				g_eventToSpeech.AnnounceValueChange(dynamic_cast<CObjectEvent*>(&*event));
				break;
			default:
				break;
		}

		event.reset();
	}

	event_queue.clear();
}
