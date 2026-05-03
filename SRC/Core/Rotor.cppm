module;
#include <array>
#include <expected>
#include <format>
#include <string>
#include <string_view>
export module Core.Rotor;
import Core.App;
import Core.Config;
import Core.Logger;
import Core.Singleton;
import Core.Speech;
import Core.SpeechSystem;

enum class ERotorCategory : unsigned char { ACTIONS, SPEECH_RATE, SPEECH_VOLUME, COUNT };

enum class ERotorAdjustmentError : unsigned char { ADJUSTED = 0, WALL };

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

template <typename T = std::string> using RotorAdjustmentResult = std::expected<T, ERotorAdjustmentError>;

template <ERotorAdjustmentDirection> using RotorAdjustmentCallback = RotorAdjustmentResult<> (*)();

struct SRotorCategoryMeta final {
	bool contextual{false};
	RotorAdjustmentCallback<ERotorAdjustmentDirection::UP> adjust_up;
	RotorAdjustmentCallback<ERotorAdjustmentDirection::DOWN> adjust_down;
	RotorAdjustmentCallback<ERotorAdjustmentDirection::ACTIVATE> adjust_activate;
};

[[nodiscard]] static consteval auto InitializeMeta(ERotorCategory category) -> SRotorCategoryMeta {
	SRotorCategoryMeta meta;

	switch (category) {
	case ERotorCategory::ACTIONS:
		meta.contextual = true;
		break;
	case ERotorCategory::SPEECH_RATE:
		meta.contextual = false;
		meta.adjust_up = []() -> RotorAdjustmentResult<> {
			auto& settings = ScreenReaderApp::GetInstance().GetSettings();
			auto& value = settings.speech.rate;
			if (value == cSpeechEngineMaxValue)
				return std::unexpected(ERotorAdjustmentError::WALL);
			++value;
			return std::format("{}", value);
		};
		meta.adjust_down = []() -> RotorAdjustmentResult<> {
			auto& settings = ScreenReaderApp::GetInstance().GetSettings();
			auto& value = settings.speech.rate;
			if (value == cSpeechEngineMinValue)
				return std::unexpected(ERotorAdjustmentError::WALL);
			--value;
			return std::format("{}", value);
		};
		break;

	case ERotorCategory::SPEECH_VOLUME:
		meta.contextual = false;
		meta.adjust_up = []() -> RotorAdjustmentResult<> {
			auto& settings = ScreenReaderApp::GetInstance().GetSettings();
			auto& value = settings.speech.volume;
			if (value == cSpeechEngineMaxValue)
				return std::unexpected(ERotorAdjustmentError::WALL);
			++value;
			return std::format("{}", value);
		};
		meta.adjust_down = []() -> RotorAdjustmentResult<> {
			auto& settings = ScreenReaderApp::GetInstance().GetSettings();
			auto& value = settings.speech.volume;
			if (value == cSpeechEngineMinValue)
				return std::unexpected(ERotorAdjustmentError::WALL);
			--value;
			return std::format("{}", value);
		};
		break;

	case ERotorCategory::COUNT:
		break;
	}

	return meta;
}

using RotorCategoryMetaArray = std::array<SRotorCategoryMeta, static_cast<size_t>(ERotorCategory::COUNT)>;

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
			if (m_category < m_categories.size() - 1)
				++m_category;
			else
				m_category = 0;
		}
		m_categoryAnnounced = false;
		m_adjustmentAnnounced = false;
		Output();
	}
};
