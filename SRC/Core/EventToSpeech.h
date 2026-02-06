// Event to speech.
#pragma once
#include "Event.h"
#include "Singleton.h"
#include "Sral.hpp"
#include <string>
#include "App.h"
#include "Utils.h"

/*
This is the final step of object event processing. Announce it.
*/
class CEventToSpeech final {
	DeclareSingleton(CEventToSpeech);
	bool m_parentAnnounced{false}; // Regarding parentAnnounce* I haven't decided yet.

	/*
	Sometimes speech events can arrive too quickly.
	We'll try to filter out events that arrive too frequently to avoid overloading the speech engine and to try to eliminate unnecessary information.
	*/
	static constexpr const uint64_t cSpeechFilterTimeMs = 10;
	CTimer m_speechFilterTimer;

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

	[[nodiscard]] inline bool Filter() {
		if (m_speechFilterTimer.Elapsed() > cSpeechFilterTimeMs) {
			m_speechFilterTimer.Restart();
			return false;
		}

		return true;
	}
};

#define g_eventToSpeech CSingleton<CEventToSpeech>::GetInstance() // Global instance.

