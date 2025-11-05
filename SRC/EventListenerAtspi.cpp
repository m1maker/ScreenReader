// AT-SPI's event listener implementation.
#include "EventListenerAtspi.h"
#include "ObjectAtspi.h"
#include "EventHandler.h"
#include "Logger.h"

/*
AT-SPI has a listener where you need to register the required events one by one and then process them with a callback.
*/
void CEventListenerAtspi::OnEventCallback(AtspiEvent* event, void* user_data) {
	[[maybe_unused]] CScopedCategory _("ATSPI event callback");
	g_logger.Log(CLogger::DEBUG, "Begin event processing");
	if (!event || !user_data || !event->type) [[unlikely]] {
		g_logger.Log(CLogger::DEBUG, "One or more pointers required by event handler was nullptr");
		return;
	}

	g_logger.Log(CLogger::DEBUG, "The event type is " + std::string(event->type));

	CEventListenerAtspi* listener = static_cast<CEventListenerAtspi*>(user_data);

	IEvent::EEventType type = GetEventTypeFromString(event->type); // The most important thing is to determine the event type.

	switch (type) { // Then send it to the handler
		/*
		It looks like in the future this will all be CObjectEvent, but just in case, we'll use switch-case for now.
		*/
		case IEvent::FOCUS_GAINED:
		case IEvent::PARENT_UPDATED:
		case IEvent::STATE_CHANGED:
		case IEvent::VALUE_CHANGED: {
			auto to_post = std::make_shared<CObjectEvent>();
			to_post->type = type;
			/*
			Here's the IEvent::now flag. It's currently used to determine whether to interrupt the speaker or wait for their turn.
			*/
			to_post->now = true;
			to_post->object = std::make_shared<CObjectAtspi>(event->source);
			listener->Post(to_post);
			break;
		}
		default: break;
	}

	g_logger.Log(CLogger::DEBUG, "End event processing");
}

CEventListenerAtspi::CEventListenerAtspi() : m_listener(atspi_event_listener_new(&CEventListenerAtspi::OnEventCallback, this, nullptr)) {
	[[maybe_unused]] CScopedCategory _("ATSPI event listener");
	if (!m_listener) {
		g_logger.Log(CLogger::ERROR, "Failed to register the listener");
		return;
	}

	/*
	AT-SPI has a listener where you need to register the required events one by one.
	But there's one very strange thing: all of these types are strings. They're defined in EventListenerAtspi.h.
	*/
	GError* error = nullptr;
	for (auto [atspi_event_type, event_type] : cAtspiEventTypeMap) {
		atspi_event_listener_register(&*m_listener, atspi_event_type.c_str(), &error);
		if (error) {
			g_logger.Log(CLogger::ERROR, "Failed to register event: " + atspi_event_type + std::string(error->message));
		}
	}
}

void CEventListenerAtspi::Post(std::shared_ptr<IEvent> event) {
	// Just push and handle.
	m_eventQueue.push_back(event);
	g_eventHandler.Handle();
}

[[nodiscard]] EventQueue& CEventListenerAtspi::RequestQueue() { // Required by CEventHandler.
	return m_eventQueue;
}

