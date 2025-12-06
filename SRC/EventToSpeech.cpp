// Event to speech.
#include "EventToSpeech.h"
#include "Object.h"
#include "EventHandler.h"
#include "Logger.h"

/*
This is the final step of object event processing. Announce it.
*/
void CEventToSpeech::AnnounceWhereAmI() {
	/*
	There's currently no general way to get a focused window. But we'll try to find one.

	This will be a common practice when we send artificial events.
	Since the event doesn't post, we don't want to duplicate the focus change announcer code.
	*/
	CObjectEvent object_event;
	object_event.object = FindFocusedObject(GetDesktopObject());
	auto listener = g_eventHandler.GetListener();
	if (!listener) [[unlikely]] return;

	CEvent to_post(std::move(object_event), CEvent::FOCUS_GAINED, false);
	listener->Post(to_post);
}

// Various Announcers
void CEventToSpeech::AnnounceFocusChange(CEvent& event) {
	auto object_event = event.GetAs<CObjectEvent>();
	if (!object_event.has_value()) {
		g_logger.Log(CLogger::ERROR, "Announcer", "Bad access to object event");
		return;
	}

	g_logger.Log(CLogger::DEBUG, "Focus", DumpObjectToString(object_event.value().object));

	/*
	So, the parent updated event is considered focus changed, but it should not be interrupted by subsequent focus gained events.
	*/
	if (event.GetType() == CEvent::PARENT_UPDATED) {
		m_parentAnnounced = true;
		/*
		When processing the "parent updated" event, AT-SPI can, for some reason, send an event with "parent updated" but no associated object.
		Often, these are menu items and the like.
		Let's try to catch those objects that, in 99% of cases, won't be parents of any other elements.
		*/
		if (!IObject::IsValidParent(object_event.value().object->GetType())) {
			//m_parentAnnounced = false;
			return;
		}
	}

	std::string announcement = object_event.value().object->GetName();
	/*
	I don't know what to do here yet, but I hope I can eventually standardize this behavior.
	When I'm in Caja Explorer, the file/folder name isn't announced in a tree view.
	When I look at it with Orca's object navigator, I see that the first child is blank, and to the right is the file name and other data, such as size, etc.
	Then it says it's the name column header, and then the name again, and so on.
	Now we'll try to find a nearby object that has a name.
	*/
	if (announcement.empty()) {
		auto children = object_event.value().object->GetChildren();
		for (const auto& child : children) {
			if (!child->GetName().empty()) {
				CObjectEvent object_event_to_announce;
				object_event_to_announce.object = child;

				CEvent to_announce(std::move(object_event_to_announce), event.GetType(), event.GetNow());
				AnnounceFocusChange(to_announce);
				return;
			}
		}
	}

	auto type = object_event.value().object->GetType();
	if (type != IObject::UNKNOWN) {
		announcement += cSeparator + IObject::GetTypeName(object_event.value().object->GetType());
	}

	if (type == IObject::SLIDER) {
		announcement += cSeparator + std::to_string(object_event.value().object->GetCurrentValue());
	}
	else if (type == IObject::TEXT_FIELD) {
		announcement += cSeparator + object_event.value().object->GetText();
	}

	auto state_names = IObject::GetStateNames(object_event.value().object->GetType(), object_event.value().object->GetState());
	for (std::string& state_name : state_names) {
		announcement += cSeparator + state_name;
	}

	/*
	Now we determine whether to ignore the "now" flag.
	If focus has been gained after the "parent updated" event, then we never interrupt the speech.
	Also, all subsequent children up to the final one should not be interrupted, but I can't do this now.
	*/
	m_speaker->Speak(announcement ,(event.GetType() == CEvent::FOCUS_GAINED && m_parentAnnounced) || event.GetType() == CEvent::PARENT_UPDATED ? false : event.GetNow());
	m_speaker->Speak(object_event.value().object->GetDescription(), false);

	if (event.GetType() != CEvent::PARENT_UPDATED) m_parentAnnounced = false;
}

void CEventToSpeech::AnnounceValueChange(CEvent& event) {
	auto object_event = event.GetAs<CObjectEvent>();
	if (!object_event.has_value()) {
		g_logger.Log(CLogger::ERROR, "Announcer", "Bad access to object event");
		return;
	}

	if (object_event.value().object->GetType () != IObject::SLIDER || !object_event.value().object->HasState(IObject::FOCUSED)) return;
	m_speaker->Speak(std::to_string(object_event.value().object->GetCurrentValue()), event.GetNow());
}

void CEventToSpeech::AnnounceStateChange(CEvent& event) {
	auto object_event = event.GetAs<CObjectEvent>();
	if (!object_event.has_value()) {
		g_logger.Log(CLogger::ERROR, "Announcer", "Bad access to object event");
		return;
	}

	if (!object_event.value().object->HasState(IObject::FOCUSED)) {
		AnnounceFocusChange(event);
		return;
	}
	std::string announcement = "";
	auto state_names = IObject::GetStateNames(object_event.value().object->GetType(), object_event.value().object->GetState());
	for (std::string& state_name : state_names) {
		announcement += cSeparator + state_name;
	}

	m_speaker->Speak(announcement, event.GetNow());
}

void CEventToSpeech::AnnounceCursorMove(CEvent& event) {
	auto object_event = event.GetAs<CObjectEvent>();
	if (!object_event.has_value()) {
		g_logger.Log(CLogger::ERROR, "Announcer", "Bad access to object event");
		return;
	}

	m_speaker->Speak(object_event.value().object->GetText(true), event.GetNow());
}
