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
	/*
	So, the parent updated event is considered focus changed, but it should not be interrupted by subsequent focus gained events.
	*/
	if (event->type == IEvent::PARENT_UPDATED) {
		m_parentAnnounced = true;
		/*
		When processing the "parent updated" event, AT-SPI can, for some reason, send an event with "parent updated" but no associated object.
		Often, these are menu items and the like.
		Let's try to catch those objects that, in 99% of cases, won't be parents of any other elements.
		*/
		if (!IObject::IsValidParent(event->object->GetType())) {
			//m_parentAnnounced = false;
			return;
		}
	}

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

	/*
	Now we determine whether to ignore the "now" flag.
	If focus has been gained after the "parent updated" event, then we never interrupt the speech.
	Also, all subsequent children up to the final one should not be interrupted, but I can't do this now.
	*/
	m_speaker->Speak(announcement ,(event->type == IEvent::FOCUS_GAINED && m_parentAnnounced) || event->type == IEvent::PARENT_UPDATED ? false : event->now);
	m_speaker->Speak(event->object->GetDescription(), false);

	if (event->type != IEvent::PARENT_UPDATED) m_parentAnnounced = false;
}

void CEventToSpeech::AnnounceValueChange(CObjectEvent* event) {
	if (event->object->GetType () != IObject::SLIDER) return;
	m_speaker->Speak(std::to_string(event->object->GetCurrentValue()), event->now);
}
