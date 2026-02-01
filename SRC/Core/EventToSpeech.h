// Event to speech.
#pragma once
#include "Event.h"
#include "Singleton.h"
#include "Sral.hpp"
#include <string>
#include "App.h"

/*
This is the final step of object event processing. Announce it.
*/
class CEventToSpeech final {
	DeclareSingleton(CEventToSpeech);
	bool m_parentAnnounced{false}; // Regarding parentAnnounce* I haven't decided yet.
public:

	static inline std::string cSeparator = "  "; // This is a separator for name, type and state.	
private:
	explicit CEventToSpeech() {}
	~CEventToSpeech() = default;

public:
	void AnnounceWhereAmI();

	void AnnounceFocusChange(CEvent& event);
	void AnnounceValueChange(CEvent& event);
	void AnnounceStateChange(CEvent& event);
	void AnnounceSelectionChange(CEvent& event);

	void AnnounceCursorMove(CEvent& event);

	inline void ParentUpdated() { m_parentAnnounced = false; }
};

#define g_eventToSpeech CSingleton<CEventToSpeech>::GetInstance() // Global instance.

