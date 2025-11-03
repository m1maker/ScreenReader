#include "EventToSpeech.h"
#include "Object.h"
#include "EventHandler.h"

void CEventToSpeech::AnnounceWhereAmI() {
	auto event = std::make_shared<CObjectEvent>();
	auto listener = g_eventHandler.GetListener();
	if (!event || !listener) [[unlikely]] return;

	event->type = IEvent::FOCUS_GAINED;
	event->object = GetDesktopObject();
	listener->Post(event);
}

void CEventToSpeech::AnnounceFocusChange(CObjectEvent* event) {
	m_speaker->Speak(event->object->GetName(), false);
	m_speaker->Speak(IObject::GetTypeName(event->object->GetType()), false);
}
