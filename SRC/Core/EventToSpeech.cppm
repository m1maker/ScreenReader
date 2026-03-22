// Event to speech.
module;

#include <array>
#include <memory_resource>
#include <string>
#include <vector>
export module Core.EventToSpeech;
import Core.Environment;
import Core.Event;
import Core.FocusManager;
import Core.SpeechSystem;
import Proxies.Object;

/*
This is the final step of object event processing. Announce it.
*/
export class EventToSpeech final {
	static constexpr size_t cBufferSize = 1024;
	alignas(std::max_align_t) std::array<std::byte, cBufferSize> m_buffer;
	std::pmr::monotonic_buffer_resource m_pool{m_buffer.data(), m_buffer.size()};

	CSpeechSystem& m_speechSystem;

	bool m_isWhereAmIOperation{false};

	CFocusManager& m_focusManager;
	std::pmr::vector<CObjectProxy> m_contextChain;

	inline void Separate(std::pmr::string& out) { out += cSeparator; }

	void BuildFocusAnnouncement(std::pmr::string& out, CObjectProxy obj, bool require_all = false);
	void BuildStateAnnouncement(std::pmr::string& out, CObjectProxy obj, bool require_all = false);
	void BuildValueAnnouncement(std::pmr::string& out, CObjectProxy obj);
	void BuildTextAnnouncement(std::pmr::string& out, CObjectProxy obj);

public:
	static constexpr std::string_view cSeparator = "  "; // This is a separator for name, type and state.
private:
	explicit EventToSpeech()
		: m_focusManager(CFocusManager::GetInstance()), m_speechSystem(CSpeechSystem::GetInstance()) {}
	~EventToSpeech() = default;

public:
	static auto& GetInstance() {
		static EventToSpeech instance;
		return instance;
	}

	/*[[candiscard]]*/ auto AnnounceWhereAmI() -> bool;

	void AnnounceFocusChange(CObjectProxy obj, bool interrupt = false);
	void AnnounceValueChange(CObjectProxy obj, bool interrupt = false);
	void AnnounceStateChange(CObjectProxy obj, bool interrupt = false);
	void AnnounceSelectionChange(CObjectProxy obj, bool interrupt = false);

	void AnnounceCursorMove(CObjectProxy obj, bool interrupt = false);
};
