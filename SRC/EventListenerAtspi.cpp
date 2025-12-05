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

void CEventListenerAtspi::OnDeviceKeyEventCallback([[maybe_unused]] AtspiDevice* device, gboolean pressed, [[maybe_unused]] guint keycode, [[maybe_unused]] guint keysym, guint modifiers, [[maybe_unused]] const gchar* key_string, void* user_data) {
	/*
	Since `atspi_event_main` is the main function of `CPlatformDependentWorkerLinux::Loop();`, we must check in the event if g_running is false, then we do `atspi_event_quit()`;.
	*/
	if (!g_running.load()) {
		atspi_event_quit();
		return;
	}

	[[maybe_unused]] CScopedCategory _("ATSPI device event callback");
	if (!user_data) {
		g_logger.Log(CLogger::DEBUG, "One or more pointers required by event handler was nullptr");
		return;
	}

	CEventListenerAtspi* listener = static_cast<CEventListenerAtspi*>(user_data);

	auto to_post = std::make_shared<CKeyboardEvent>();
	to_post->type = pressed ? IEvent::KEY_PRESSED : IEvent::KEY_RELEASED;
	to_post->hotkey.keycode = GdkKeysymToKeyboardEventKeycode(keysym);
	to_post->hotkey.modifiers = GdkModifierToKeyboardEventModifiers(modifiers);
	listener->Post(to_post);
}

CEventListenerAtspi::CEventListenerAtspi() : 
	m_objectEventListener(atspi_event_listener_new(&CEventListenerAtspi::OnObjectEventCallback, this, nullptr)), 
	m_device(atspi_device_new()) {
	[[maybe_unused]] CScopedCategory _("ATSPI event listener");
	if (!m_objectEventListener) [[unlikely]] {
		g_logger.Log(CLogger::ERROR, "Failed to register the object event listener");
		return;
	}

	if (!m_device) [[unlikely]] {
		g_logger.Log(CLogger::ERROR, "Failed to register the device event listener");
		return;
	}

	/*
	AT-SPI has a listener where you need to register the required events one by one.
	*/
	GError* error = nullptr;
	for (const auto& [atspi_event_type, event_type] : cAtspiObjectEventTypeMap) {
		atspi_event_listener_register(m_objectEventListener, atspi_event_type.c_str(), &error);
		if (error) {
			g_logger.Log(CLogger::ERROR, "Failed to register event: " + atspi_event_type + std::string(error->message));
			g_error_free(error);
			error = nullptr;
		}
	}

	atspi_device_add_key_watcher(m_device, &CEventListenerAtspi::OnDeviceKeyEventCallback, this, nullptr);
}

void CEventListenerAtspi::Post(std::shared_ptr<IEvent> event) {
	// Just push and handle.
	m_eventQueue.push_back(event);
	g_eventHandler.Handle();
}

[[nodiscard]] EventQueue& CEventListenerAtspi::RequestQueue() { // Required by CEventHandler.
	return m_eventQueue;
}

