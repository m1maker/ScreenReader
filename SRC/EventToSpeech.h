#pragma once
#include "Event.h"
#include "SpeechEngine.h"
#include "Singleton.h"
#include <string>

class CEventToSpeech final {
	bool m_parentAnnounced{false};
	std::shared_ptr<Sral::Engine> m_speaker;
public:

	static inline std::string cSeparator = "  ";

	explicit CEventToSpeech() : m_speaker(g_speechEngine.GetSpeaker()) {}
	~CEventToSpeech() = default;

	void AnnounceWhereAmI();

	void AnnounceFocusChange(CObjectEvent* event);
	void AnnounceValueChange(CObjectEvent* event);

	inline void ParentUpdated() { m_parentAnnounced = false; }
};

#define g_eventToSpeech CSingleton<CEventToSpeech>::GetInstance()

