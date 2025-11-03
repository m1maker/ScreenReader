#include "EventToSpeech.h"
#include "Object.h"
#include "EventHandler.h"

void CEventToSpeech::AnnounceWhereAmI() {
	auto event = std::make_shared<CObjectEvent>();
	auto listener = g_eventHandler.GetListener();
	if (!event || !listener) [[unlikely]] return;

	event->type = IEvent::FOCUS_GAINED;
	event->now = false;
	event->object = GetDesktopObject();
	listener->Post(event);
}

void CEventToSpeech::AnnounceFocusChange(CObjectEvent* event) {
	std::string announcement = event->object->GetName();

	if (event->object->GetType() != IObject::UNKNOWN) {
		announcement += cSeparator + IObject::GetTypeName(event->object->GetType());
	}

	auto state_names = IObject::GetStateNames(event->object->GetState());
	for (std::string& state_name : state_names) {
		announcement += cSeparator + state_name;
	}

	m_speaker->Speak(announcement ,event->now);
	m_speaker->Speak(event->object->GetDescription(), false);
}
