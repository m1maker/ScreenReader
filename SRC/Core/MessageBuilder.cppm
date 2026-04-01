module;
#include <array>
#include <cstdint>
#include <cstdio>
#include <format>
#include <memory_resource>
#include <string>
#include <string_view>
#include <vector>
export module Core.MessageBuilder;
import Core.Config;
import Core.Logger;
import Core.Singleton;
import Core.Text;
import Core.Utterance;
import Proxies.Object;

export class CMessage final {
	bool m_ssml;
	std::pmr::memory_resource* m_pool;
	std::pmr::string m_content;
	mutable CUtterance m_utterance;
	friend class MessageBuilder;

	explicit CMessage(bool ssml, std::pmr::memory_resource* pool)
		: m_ssml(ssml), m_pool(pool), m_content(m_pool), m_utterance(m_content) {
		if (m_ssml)
			m_utterance.Begin();
	}

public:
	template <typename... Args> void Append(std::format_string<Args...> fmt, Args&&... args) {
		if (!m_ssml)
			std::format_to(std::back_inserter(m_content), fmt, std::forward<Args>(args)...);
		else
			m_utterance.Text(std::format(fmt, std::forward<Args>(args)...));
	}

	void Separate() {
		if (!m_ssml)
			m_content += "  ";
		else
			m_utterance.Break("1ms");
	}
	inline void ApplyUtteranceParameters(UtteranceParameters parameters) {
		if (!m_ssml)
			return;
		m_utterance.Break(parameters.pause_before)
			.Rate(parameters.rate)
			.Pitch(parameters.pitch)
			.Volume(parameters.volume);
	}

	[[nodiscard]] operator std::string_view() const {
		m_utterance.End();
		return m_content;
	}
};

export class MessageBuilder final : TModule<"MessageBuilder">, public TSingleton<MessageBuilder> {
	static constexpr size_t cBufferSize = 1024;
	alignas(std::max_align_t) std::array<std::byte, cBufferSize> m_buffer;
	std::pmr::monotonic_buffer_resource m_pool{m_buffer.data(), m_buffer.size()};
	SpeechParameters& m_speechParameters;

	bool m_ssml{false};

public:
	MessageBuilder();

	void FindAnnouncementInHierarchy(
		CMessage& message, CObjectProxy obj, bool recursive = true, bool collect_all_labels = true);
	void FindAnnouncementOfCursorPosition(
		CMessage& message, CTextProviderProxy provider, ETextGranularity& granularity);
	[[nodiscard]] auto CreateMessage() -> CMessage { return CMessage(m_ssml, &m_pool); }

	void BuildFocusAnnouncement(CMessage& message, CObjectProxy obj, bool require_all = false);
	void BuildStateAnnouncement(CMessage& message, CObjectProxy obj, bool require_all = false);
	void BuildSelectionAnnouncement(CMessage& message, CObjectProxy obj);
	void BuildValueAnnouncement(CMessage& message, CObjectProxy obj);

	void BuildNameAnnouncement(CMessage& message, CObjectProxy obj);
	void BuildDescriptionAnnouncement(CMessage& message, CObjectProxy obj);
	void BuildTextAnnouncement(CMessage& message, CObjectProxy obj);

	void BuildTextSelectionAnnouncement(CMessage& message, CObjectProxy obj);
	void BuildCursorAnnouncement(CMessage& message, CObjectProxy obj);
};
