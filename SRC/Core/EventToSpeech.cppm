// Event to speech.
module;

#include <memory_resource>
#include <string>
#include <vector>
export module Core.EventToSpeech;
import Core.Environment;
import Core.Event;
import Core.FocusManager;
import Core.ObjectAccessor;
import Core.SpeechSystem;
import Core.SsmlUtterance;

/*
This is the final step of object event processing. Announce it.
*/
export class CEventToSpeech final {
	bool m_parentAnnounced{false}; // Regarding parentAnnounce* I haven't decided yet.
	CSsmlUtterance m_ssmlUtterance;
	CSpeechSystem& m_speechSystem;

	bool m_isWhereAmIOperation{false};

	CFocusManager& m_focusManager;
	std::pmr::vector<CObjectAccessor> m_contextChain;

public:
	static constexpr inline std::string_view cSeparator = "  "; // This is a separator for name, type and state.
private:
	explicit CEventToSpeech()
		: m_focusManager(CFocusManager::GetInstance()), m_speechSystem(CSpeechSystem::GetInstance()) {}
	~CEventToSpeech() = default;

public:
	static auto& GetInstance() {
		static CEventToSpeech instance;
		return instance;
	}

	/*[[candiscard]]*/ auto AnnounceWhereAmI() -> bool;

	void AnnounceFocusChange(CEvent& event);
	void AnnounceValueChange(CEvent& event);
	void AnnounceStateChange(CEvent& event);
	void AnnounceSelectionChange(CEvent& event);

	void AnnounceCursorMove(CEvent& event);

	inline void ParentUpdated() { m_parentAnnounced = false; }
};
