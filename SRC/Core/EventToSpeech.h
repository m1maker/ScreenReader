// Event to speech.
#pragma once
#include "App.h"
#include "Event.h"
#include "Singleton.h"
#include "Sral.hpp"
#include "Utils.h"

#include <memory_resource>
#include <string>
#include <vector>

/*
This is the final step of object event processing. Announce it.
*/
class CEventToSpeech final {
	DeclareSingleton(CEventToSpeech);
	bool m_parentAnnounced{false}; // Regarding parentAnnounce* I haven't decided yet.

	/*
	Sometimes speech events can arrive too quickly.
	We'll try to filter out events that arrive too frequently to avoid overloading the speech engine and to try to
	eliminate unnecessary information.
	*/
	static constexpr const uint64_t cSpeechFilterTimeMs = 10;
	CTimer m_speechFilterTimer;

	std::pmr::vector<std::shared_ptr<IObject>> m_contextChain;

public:
	static constexpr inline std::string_view cSeparator = "  "; // This is a separator for name, type and state.
private:
	explicit CEventToSpeech() {}
	~CEventToSpeech() = default;

public:
	/*[[candiscard]]*/ auto AnnounceWhereAmI() -> bool;

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
