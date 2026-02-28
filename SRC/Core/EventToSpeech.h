// Event to speech.
#pragma once
#include "App.h"
#include "Event.h"
#include "Singleton.h"
#include "Sral.hpp"

#include <memory_resource>
#include <string>
#include <vector>
import Core.FocusManager;

/*
This is the final step of object event processing. Announce it.
*/
class CEventToSpeech final {
	DeclareSingleton(CEventToSpeech);

	void SpeakObject(CObject object);
	bool m_parentAnnounced{false}; // Regarding parentAnnounce* I haven't decided yet.

	bool m_isWhereAmIOperation{false};

	CFocusManager& m_focusManager;
	std::pmr::vector<CObject> m_contextChain;

public:
	static constexpr inline std::string_view cSeparator = "  "; // This is a separator for name, type and state.
private:
	explicit CEventToSpeech() : m_focusManager(CFocusManager::GetInstance()) {}
	~CEventToSpeech() = default;

public:
	/*[[candiscard]]*/ auto AnnounceWhereAmI() -> bool;

	void AnnounceFocusChange(CEvent& event);
	void AnnounceValueChange(CEvent& event);
	void AnnounceStateChange(CEvent& event);
	void AnnounceSelectionChange(CEvent& event);

	void AnnounceCursorMove(CEvent& event);

	inline void ParentUpdated() { m_parentAnnounced = false; }
};

#define g_eventToSpeech CSingleton<CEventToSpeech>::GetInstance() // Global instance.
