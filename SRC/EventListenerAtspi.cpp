// AT-SPI's event listener implementation.
#include "EventListenerAtspi.h"
#include "ObjectAtspi.h"
#include "EventHandler.h"
#include "Logger.h"
#include "AppState.h"

/*
AT-SPI has a listener where you need to register the required events one by one and then process them with a callback.
*/
void CEventListenerAtspi::OnObjectEventCallback(AtspiEvent* event, void* user_data) {
	/*
	Since `atspi_event_main` is the main function of `CPlatformDependentWorkerLinux::Loop();`, we must check in the event if g_running is false, then we do `atspi_event_quit()`;.
	*/
	if (!g_running.load()) {
		atspi_event_quit();
		return;
	}

	[[maybe_unused]] CScopedCategory _("ATSPI object event callback");
	if (!event || !user_data || !event->type) [[unlikely]] {
		g_logger.Log(CLogger::DEBUG, "One or more pointers required by event handler was nullptr");
		return;
	}

	CEventListenerAtspi* listener = static_cast<CEventListenerAtspi*>(user_data);

	IEvent::EEventType type = GetEventTypeFromString(event->type); // The most important thing is to determine the event type.

	switch (type) { // Then send it to the handler
		/*
		It looks like in the future this will all be CObjectEvent, but just in case, we'll use switch-case for now.
		*/
		case IEvent::FOCUS_GAINED:
		case IEvent::CURSOR_MOVED:
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
}

gboolean CEventListenerAtspi::OnDeviceEventCallback(AtspiDeviceEvent* event, void* user_data) {
	/*
	Since `atspi_event_main` is the main function of `CPlatformDependentWorkerLinux::Loop();`, we must check in the event if g_running is false, then we do `atspi_event_quit()`;.
	*/
	if (!g_running.load()) {
		atspi_event_quit();
		return FALSE;
	}

	[[maybe_unused]] CScopedCategory _("ATSPI device event callback");
	if (!event || !user_data) {
		g_logger.Log(CLogger::DEBUG, "One or more pointers required by event handler was nullptr");
		return FALSE;
	}

	CEventListenerAtspi* listener = static_cast<CEventListenerAtspi*>(user_data);

	auto to_post = std::make_shared<CKeyboardEvent>();
	to_post->type = AtspiEventTypeToEventType(event->type);
	to_post->keycode = LinuxKeycodeToKeyboardEventKeycode(event->hw_code);
	// Modifiers not complete.
	g_logger.Log(CLogger::INFO, CKeyboardEvent::GetKeycodeName(to_post->keycode));
	listener->Post(to_post);
	return FALSE;
}

CEventListenerAtspi::CEventListenerAtspi() : 
	m_objectEventListener(atspi_event_listener_new(&CEventListenerAtspi::OnObjectEventCallback, this, nullptr)), 
	m_deviceEventListener(atspi_device_listener_new(&CEventListenerAtspi::OnDeviceEventCallback, this, nullptr)) {
	[[maybe_unused]] CScopedCategory _("ATSPI event listener");
	if (!m_objectEventListener) [[unlikely]] {
		g_logger.Log(CLogger::ERROR, "Failed to register the object event listener");
		return;
	}

	if (!m_deviceEventListener) [[unlikely]] {
		g_logger.Log(CLogger::ERROR, "Failed to register the device event listener");
		return;
	}

	/*
	AT-SPI has a listener where you need to register the required events one by one.
	*/
	GError* error = nullptr;
	for (auto [atspi_event_type, event_type] : cAtspiObjectEventTypeMap) {
		atspi_event_listener_register(m_objectEventListener, atspi_event_type.c_str(), &error);
		if (error) {
			g_logger.Log(CLogger::ERROR, "Failed to register event: " + atspi_event_type + std::string(error->message));
			g_error_free(error);
		}
	}

	for (auto event_type : cAtspiDeviceListenerEventTypes) {
		atspi_register_device_event_listener(m_deviceEventListener, event_type, nullptr, &error);
		if (error) {
			g_logger.Log(CLogger::ERROR, "Failed to register event: " + std::to_string(event_type) + std::string(error->message));
			g_error_free(error);
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

