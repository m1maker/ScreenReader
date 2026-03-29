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
	std::pmr::string m_content{&m_pool};
	CUtterance m_utterance{m_content};

	bool m_ssml{false};

	inline void Separate() { m_content += cSeparator; }

	inline void Append(std::string_view text) {
		if (m_ssml) {
			m_utterance.Text(text);
		}
		else
			m_content += text;
	}

	inline void ApplyUtteranceParameters(UtteranceParameters parameters) {
		m_utterance.Break(parameters.pause_before)
			.Rate(parameters.rate)
			.Pitch(parameters.pitch)
			.Volume(parameters.volume)
			.Break(parameters.pause_after);
	}

	MessageBuilder();

public:
	static auto& GetInstance() {
		static MessageBuilder instance;
		return instance;
	}
	void FindAnnouncementInHierarchy(CObjectProxy obj, bool recursive = true, bool collect_all_labels = true);
	void FindAnnouncementOfCursorPosition(CTextProviderProxy provider, ETextGranularity& granularity);

	inline void Reset() {
		if (m_ssml) {
			m_utterance.Clear();
		}
		else
			m_content.clear();
	}

	void BuildFocusAnnouncement(CObjectProxy obj, bool require_all = false);
	void BuildStateAnnouncement(CObjectProxy obj, bool require_all = false);
	void BuildValueAnnouncement(CObjectProxy obj);
	void BuildTextAnnouncement(CObjectProxy obj);

	[[nodiscard]] operator std::string_view() const { return std::string_view(m_content); }

	static constexpr std::string_view cSeparator = "  "; // This is a separator for name, type and state.
};
