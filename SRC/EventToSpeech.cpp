// Event to speech.
#include "EventToSpeech.h"
#include "Object.h"
#include "EventHandler.h"

/*
This is the final step of object event processing. Announce it.
*/
void CEventToSpeech::AnnounceWhereAmI() {
	/*
	There's currently no general way to get a focused window. But we'll try to find one.

	This will be a common practice when we send artificial events.
	Since the event doesn't post, we don't want to duplicate the focus change announcer code.
	*/
	auto event = std::make_shared<CObjectEvent>();
	auto listener = g_eventHandler.GetListener();
	if (!event || !listener) [[unlikely]] return;

	event->type = IEvent::FOCUS_GAINED;
	event->now = false; // Don't interrupt speech.
	event->object = FindFocusedObject(GetDesktopObject());
	listener->Post(event);
}

// Various Announcers
void CEventToSpeech::AnnounceFocusChange(CObjectEvent* event) {
	std::string announcement = event->object->GetName();

	auto type = event->object->GetType();
	if (type != IObject::UNKNOWN) {
		announcement += cSeparator + IObject::GetTypeName(event->object->GetType());
	}

	if (type == IObject::SLIDER) {
		announcement += cSeparator + std::to_string(event->object->GetCurrentValue());
	}

	auto state_names = IObject::GetStateNames(event->object->GetType(), event->object->GetState());
	for (std::string& state_name : state_names) {
		announcement += cSeparator + state_name;
	}

	m_speaker->Speak(announcement ,event->now);
	m_speaker->Speak(event->object->GetDescription(), false);
}

void CEventToSpeech::AnnounceValueChange(CObjectEvent* event) {
	if (event->object->GetType () != IObject::SLIDER) return;
	m_speaker->Speak(std::to_string(event->object->GetCurrentValue()), event->now);
}
