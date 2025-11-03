#include "EventToSpeech.h"

void CEventToSpeech::AnnounceFocusChange(CObjectEvent* event) {
	m_speaker->Speak(event->object->GetName(), false);
	m_speaker->Speak(IObject::GetTypeName(event->object->GetType()), false);
}
