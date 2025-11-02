#include "EventToSpeech.h"

void CEventToSpeech::AnnounceFocusChange(CObjectEvent* event) {
	m_speaker->Speak(event->object->GetName(), true);
}
