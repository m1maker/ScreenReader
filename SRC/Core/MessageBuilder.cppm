module;
#include <array>
#include <cstdint>
#include <cstdio>
#include <memory_resource>
#include <string>
#include <vector>
export module Core.MessageBuilder;
import Core.Config;
import Core.Text;
import Core.Utterance;
import Proxies.Object;

export class MessageBuilder final {
	static constexpr size_t cBufferSize = 1024;
	alignas(std::max_align_t) std::array<std::byte, cBufferSize> m_buffer;
	std::pmr::monotonic_buffer_resource m_pool{m_buffer.data(), m_buffer.size()};

	std::pmr::string m_ssmlContent{&m_pool};
	CUtterance m_utterance{m_ssmlContent};

	static inline void Separate(std::pmr::string& out) { out += cSeparator; }

	inline void ApplyUtteranceParameters(UtteranceParameters parameters) {
		m_utterance.Break(parameters.pause_before)
			.Rate(parameters.rate)
			.Pitch(parameters.pitch)
			.Volume(parameters.volume)
			.Break(parameters.pause_after);
	}

public:
	static auto& GetInstance() {
		static MessageBuilder instance;
		return instance;
	}
	static void FindAnnouncementInHierarchy(
		std::pmr::string& out, CObjectProxy obj, bool recursive = true, bool collect_all_labels = true);
	static void FindAnnouncementOfCursorPosition(
		std::pmr::string& out, CTextProviderProxy provider, ETextGranularity& granularity);

	[[nodiscard]] auto CreateString() -> std::pmr::string { return std::pmr::string(&m_pool); }

	void BuildFocusAnnouncement(std::pmr::string& out, CObjectProxy obj, bool require_all = false);
	void BuildStateAnnouncement(std::pmr::string& out, CObjectProxy obj, bool require_all = false);
	void BuildValueAnnouncement(std::pmr::string& out, CObjectProxy obj);
	void BuildTextAnnouncement(std::pmr::string& out, CObjectProxy obj);
	static constexpr std::string_view cSeparator = "  "; // This is a separator for name, type and state.
};
