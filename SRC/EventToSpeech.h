// Event to speech.
#pragma once
#include "Event.h"
#include "SpeechEngine.h"
#include "Singleton.h"
#include <string>

/*
This is the final step of object event processing. Announce it.
*/
class CEventToSpeech final {
	bool m_parentAnnounced{false}; // Regarding parentAnnounce* I haven't decided yet.
	std::shared_ptr<Sral::Engine> m_speaker;
public:

	static inline std::string cSeparator = "  "; // This is a separator for name, type and state.

	explicit CEventToSpeech() : m_speaker(g_speechEngine.GetSpeaker()) {}
	~CEventToSpeech() = default;

	void AnnounceWhereAmI();

	void AnnounceFocusChange(CObjectEvent* event);
	void AnnounceValueChange(CObjectEvent* event);

	inline void ParentUpdated() { m_parentAnnounced = false; }
};

#define g_eventToSpeech CSingleton<CEventToSpeech>::GetInstance() // Global instance.

