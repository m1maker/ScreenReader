module;
#include <format>
#include <memory_resource>
#include <string>
export module Core.Message;
import Core.Config;
import Core.Utterance;

export class CMessage final {
	bool m_ssml;
	std::pmr::memory_resource* m_pool;
	std::pmr::string m_content;
	mutable CUtterance m_utterance;

public:
	explicit CMessage(bool ssml, std::pmr::memory_resource* pool)
		: m_ssml(ssml), m_pool(pool), m_content(m_pool), m_utterance(m_content) {
		if (m_ssml)
			m_utterance.Begin();
	}

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

	[[nodiscard]] auto GetBuffer() -> std::pmr::string& { return m_content; }

	[[nodiscard]] operator std::string_view() const {
		m_utterance.End();
		return m_content;
	}
};
