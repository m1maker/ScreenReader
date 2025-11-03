#pragma once
#include "Event.h"
#include "SpeechEngine.h"
#include "Singleton.h"

class CEventToSpeech final {
	bool m_parentAnnounced{false};
	std::shared_ptr<Sral::Engine> m_speaker;
public:

	explicit CEventToSpeech() : m_speaker(g_speechEngine.GetSpeaker()) {}
	~CEventToSpeech() = default;

	void AnnounceWhereAmI();

	void AnnounceFocusChange(CObjectEvent* event);

	inline void ParentUpdated() { m_parentAnnounced = false; }
};

#define g_eventToSpeech CSingleton<CEventToSpeech>::GetInstance()

