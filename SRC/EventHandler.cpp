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
			case IEvent::PARENT_UPDATED: {
				auto object_event = dynamic_cast<CObjectEvent*>(&*event);
				if (ObjectIsSame(object_event->object, m_objectHandledPrevious)) break;
				else m_objectHandledPrevious = object_event->object;
				g_eventToSpeech.AnnounceFocusChange(object_event);
				break;
			}
			case IEvent::VALUE_CHANGED: {
				auto object_event = dynamic_cast<CObjectEvent*>(&*event);
				g_eventToSpeech.AnnounceValueChange(object_event);
				break;
			}
			default:
				break;
		}
	}

	event_queue.clear();
}
