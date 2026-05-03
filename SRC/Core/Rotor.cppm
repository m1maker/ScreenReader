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

template <typename T = std::string> using RotorAdjustmentResult = std::expected<T, ERotorAdjustmentError>;

template <ERotorAdjustmentDirection> using RotorAdjustmentCallback = RotorAdjustmentResult<> (*)();

struct SRotorCategoryMeta final {
	std::string_view speech_name;
	bool contextual{false};
	RotorAdjustmentCallback<ERotorAdjustmentDirection::UP> adjust_up{nullptr};
	RotorAdjustmentCallback<ERotorAdjustmentDirection::DOWN> adjust_down{nullptr};
	RotorAdjustmentCallback<ERotorAdjustmentDirection::ACTIVATE> adjust_activate{nullptr};
};

[[nodiscard]] static consteval auto InitializeMeta(ERotorCategory category) -> SRotorCategoryMeta {
	SRotorCategoryMeta meta;

	switch (category) {
	case ERotorCategory::ACTIONS:
		meta.speech_name = "Actions";
		meta.contextual = true;
		break;
	case ERotorCategory::SPEECH_RATE:
		meta.speech_name = "Speech rate";
		meta.contextual = false;
		meta.adjust_up = []() -> RotorAdjustmentResult<> {
			auto& settings = ScreenReaderApp::GetInstance().GetSettings();
			auto& value = settings.speech.rate;
			if (value == cSpeechEngineMaxValue)
				return std::unexpected(ERotorAdjustmentError::WALL);
			++value;
			return std::to_string(value);
		};
		meta.adjust_down = []() -> RotorAdjustmentResult<> {
			auto& settings = ScreenReaderApp::GetInstance().GetSettings();
			auto& value = settings.speech.rate;
			if (value == cSpeechEngineMinValue)
				return std::unexpected(ERotorAdjustmentError::WALL);
			--value;
			return std::to_string(value);
		};
		break;

	case ERotorCategory::SPEECH_VOLUME:
		meta.speech_name = "Speech volume";
		meta.contextual = false;
		meta.adjust_up = []() -> RotorAdjustmentResult<> {
			auto& settings = ScreenReaderApp::GetInstance().GetSettings();
			auto& value = settings.speech.volume;
			if (value == cSpeechEngineMaxValue)
				return std::unexpected(ERotorAdjustmentError::WALL);
			++value;
			return std::to_string(value);
		};
		meta.adjust_down = []() -> RotorAdjustmentResult<> {
			auto& settings = ScreenReaderApp::GetInstance().GetSettings();
			auto& value = settings.speech.volume;
			if (value == cSpeechEngineMinValue)
				return std::unexpected(ERotorAdjustmentError::WALL);
			--value;
			return std::to_string(value);
		};
		break;

	case ERotorCategory::COUNT:
		break;
	}

	return meta;
}

using RotorCategoryMetaArray = std::array<SRotorCategoryMeta, static_cast<size_t>(ERotorCategory::COUNT)>;

[[nodiscard]] static consteval auto InitializeMetaArray() -> RotorCategoryMetaArray {
	RotorCategoryMetaArray array;
	for (size_t i = 0; i < static_cast<size_t>(ERotorCategory::COUNT); ++i) {
		auto meta = InitializeMeta(static_cast<ERotorCategory>(i));
		array[i] = meta;
	}
	return array;
}

static constexpr RotorCategoryMetaArray cRotorCategoryMetadata = InitializeMetaArray();

export [[nodiscard]] constexpr auto GetRotorCategoryName(ERotorCategory category) -> std::string_view {
	auto index = static_cast<size_t>(category);
	if (index < 0 || index > cRotorCategoryMetadata.size()) [[unlikely]]
		return "unknown";

	return cRotorCategoryMetadata[index].speech_name;
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
