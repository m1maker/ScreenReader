module;
#include <array>
#include <cstdint>
#include <cstdio>
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

template <class T> class TScopedBegin final {
	T& m_instance;
	friend T;
	explicit TScopedBegin(T& instance) : m_instance(instance) { m_instance.Begin(); }
	~TScopedBegin() { m_instance.End(); }
};

export class MessageBuilder final : TModule<"MessageBuilder">, public TSingleton<MessageBuilder> {
	static constexpr size_t cBufferSize = 1024;
	alignas(std::max_align_t) std::array<std::byte, cBufferSize> m_buffer;
	std::pmr::monotonic_buffer_resource m_pool{m_buffer.data(), m_buffer.size()};
	std::pmr::string m_content{&m_pool};
	CUtterance m_utterance{m_content};
	SpeechParameters& m_speechParameters;

	bool m_ssml{false};
	std::string_view m_lastBreakAfter{""};
	mutable unsigned char m_counter{0};
	friend TScopedBegin<MessageBuilder>;

	inline void Begin() {
		if (++m_counter == 1) {
			if (m_ssml)
				m_utterance.Begin();
		}
	}

	inline void End() {
		if (--m_counter == 0) {
			if (m_ssml)
				m_utterance.End();
			m_lastBreakAfter = "";
		}
	}

	inline void Separate() { m_content += cSeparator; }

	inline void Append(std::string_view text) {
		if (m_ssml) {
			m_utterance.Text(text);
			if (!m_lastBreakAfter.empty()) {
				m_utterance.Break(m_lastBreakAfter);
				m_lastBreakAfter = "";
			}
		}
		else
			m_content += text;
	}

	inline void ApplyUtteranceParameters(UtteranceParameters parameters) {
		if (!m_ssml)
			return;
		m_utterance.Break(parameters.pause_before)
			.Rate(parameters.rate)
			.Pitch(parameters.pitch)
			.Volume(parameters.volume);
		m_lastBreakAfter = parameters.pause_after;
	}

public:
	MessageBuilder();

	void FindAnnouncementInHierarchy(CObjectProxy obj, bool recursive = true, bool collect_all_labels = true);
	void FindAnnouncementOfCursorPosition(CTextProviderProxy provider, ETextGranularity& granularity);

	inline void Reset() {
		End();
		if (m_ssml) {
			m_utterance.Clear();
		}
		else
			m_content.clear();
	}

	void BuildFocusAnnouncement(CObjectProxy obj, bool require_all = false);
	void BuildStateAnnouncement(CObjectProxy obj, bool require_all = false);
	void BuildSelectionAnnouncement(CObjectProxy obj);
	void BuildValueAnnouncement(CObjectProxy obj);

	void BuildNameAnnouncement(CObjectProxy obj);
	void BuildDescriptionAnnouncement(CObjectProxy obj);
	void BuildTextAnnouncement(CObjectProxy obj);
	void BuildCursorAnnouncement(CObjectProxy obj);

	[[nodiscard]] operator std::string_view() const { return std::string_view(m_content); }

	static constexpr std::string_view cSeparator = "  "; // This is a separator for name, type and state.
};
