module;
#include <array>
#include <string_view>
export module Core.Rotor;
import Core.App;
import Core.Config;
import Core.Logger;
import Core.Singleton;
import Core.SpeechSystem;

enum class ERotorCategory : unsigned char { ACTIONS, SPEECH_RATE, SPEECH_VOLUME, COUNT };

enum class ERotorAdjustmentResult : unsigned char { WALL, ADJUSTED };

export enum class ERotorSpinDirection : unsigned char { LEFT, RIGHT };

enum class ERotorAdjustmentDirection : unsigned char { UP, DOWN, ACTIVATE };

[[nodiscard]] static constexpr inline auto GetRotorCategoryName(ERotorCategory category) -> std::string_view {
	using enum ERotorCategory;
	switch (category) {
	case ACTIONS:
		return "Actions";
	case SPEECH_RATE:
		return "Speech rate";
	case SPEECH_VOLUME:
		return "Speech volume";
	case COUNT:
		return "";
	}
}

using RotorCategoryArray = std::array<ERotorCategory, static_cast<size_t>(ERotorCategory::COUNT)>;

export class Rotor : TModule<"Rotor">, public TSingleton<Rotor> {
	RotorCategoryArray m_categories;
	size_t m_category{0};
	bool m_categoryAnnounced{false}, m_adjustmentAnnounced{false};

	void Output() {
		if (!m_categoryAnnounced) {
			SpeechSystem::GetInstance().Interrupt();
			SpeechSystem::GetInstance().Speak(GetRotorCategoryName(m_categories[m_category]));
			m_categoryAnnounced = true;
		}
	}

public:
	explicit Rotor() {
		// Read from settings in the future.
		using enum ERotorCategory;
		m_categories = {ACTIONS, SPEECH_RATE, SPEECH_VOLUME};
	}

	template <ERotorSpinDirection Direction> void Spin() {
		if constexpr (Direction == ERotorSpinDirection::LEFT) {
			if (m_category == 0)
				m_category = m_categories.size() - 1;
			else
				--m_category;
		}
		else if constexpr (Direction == ERotorSpinDirection::RIGHT) {
			if (m_category < m_categories.size())
				++m_category;
			else
				m_category = 0;
		}
		m_categoryAnnounced = false;
		m_adjustmentAnnounced = false;
		Output();
	}
};
