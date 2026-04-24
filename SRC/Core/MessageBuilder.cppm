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
import Core.Message;
import Core.Singleton;
import Core.Text;
import Core.Utterance;
import Proxies.Object;

export class MessageBuilder final : TModule<"MessageBuilder">, public TSingleton<MessageBuilder> {
	static constexpr size_t cBufferSize = 1024;
	alignas(std::max_align_t) std::array<std::byte, cBufferSize> m_buffer;
	std::pmr::monotonic_buffer_resource m_pool{m_buffer.data(), m_buffer.size()};
	SpeechParameters& m_speechParameters;

	bool m_ssml{false};

public:
	MessageBuilder();

	[[nodiscard]] auto GetPool() -> std::pmr::memory_resource* { return &m_pool; }

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
