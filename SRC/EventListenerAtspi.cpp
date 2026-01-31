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

	auto* listener = static_cast<CEventListenerAtspi*>(user_data);

	CEvent::EEventType type = GetEventTypeFromString(event->type); // The most important thing is to determine the event type.

	/*
	I haven't figured out exactly how these detail 1/detail 2 members work yet, but I've figured out which events don't need to be dispatched when !detail1 is.
	This list of types will likely expand once I fully understand what they are.
	*/
	if ((type == CEvent::FOCUS_GAINED || type == CEvent::SELECTION_CHANGED) && !event->detail1) return;
	switch (type) { // Then send it to the handler
		/*
		It looks like in the future this will all be CObjectEvent, but just in case, we'll use switch-case for now.
		*/
		case CEvent::FOCUS_GAINED:
		case CEvent::CURSOR_MOVED:
		case CEvent::PARENT_UPDATED:
		case CEvent::STATE_CHANGED:
		case CEvent::SELECTION_CHANGED:
		case CEvent::VALUE_CHANGED: {
			CObjectEvent object_event;
			object_event.object = g_objectCache(AtspiAccessible, CObjectAtspi).GetOrCreate(event->source);
			object_event.previous_cursor_position = *object_event.object->GetCursor();
			object_event.object->UpdateCacheByEvent(type);
			/*
			Here's the CEvent::now flag. It's currently used to determine whether to interrupt the speaker or wait for their turn.
			*/
			CEvent to_post(object_event, type, true);
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

	auto* listener = static_cast<CEventListenerAtspi*>(user_data);

	CKeyboardEvent keyboard_event;
	keyboard_event.hotkey.keycode = GdkKeysymToKeyboardEventKeycode(keysym);
	keyboard_event.hotkey.modifiers = GdkModifierToKeyboardEventModifiers(modifiers);
	CEvent to_post(std::move(keyboard_event), static_cast<CEvent::EEventType>(pressed ? CEvent::KEY_PRESSED : CEvent::KEY_RELEASED), false);
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

void CEventListenerAtspi::Post(const CEvent& event) {
	// Just push and handle.
	m_eventQueue.push_back(event);
	g_eventHandler.Handle();
}

[[nodiscard]] auto CEventListenerAtspi::RequestQueue() -> EventQueue& { // Required by CEventHandler.
	return m_eventQueue;
}

