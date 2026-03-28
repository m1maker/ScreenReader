module;
#include <map>
#include <memory>
#include <memory_resource>
#include <string>
#include <string_view>
export module Core.Utterance;

export class CUtterance final {
	size_t m_prefixLength{0};
	std::pmr::string& m_ssmlContent;
	std::string_view m_currentPitch;
	std::string_view m_currentRate;
	std::string_view m_currentVolume;
	std::string_view m_currentVoice;
	bool m_inProsody{false};

	void StartProsodyIfNeeded();
	void EndProsodyIfNeeded();
	void AddAndEscapeXml(std::string_view text);

public:
	explicit CUtterance(std::pmr::string& ssml);

	auto Text(this auto&& self, std::string_view text);
	auto Break(this auto&& self, std::string_view time = "500ms");

	auto Mark(this auto&& self, std::string_view name);

	auto Pitch(this auto&& self, std::string_view pitch);
	auto Rate(this auto&& self, std::string_view rate);
	auto Volume(this auto&& self, std::string_view volume);

	auto Voice(this auto&& self, std::string_view voice);

	auto Finish(this auto&& self);
	void Clear();

	static constexpr std::string_view cPitchXlow = "x-low";
	static constexpr std::string_view cPitchLow = "low";
	static constexpr std::string_view cPitchMedium = "medium";
	static constexpr std::string_view cPitchHigh = "high";
	static constexpr std::string_view cPitchXhigh = "x-high";
	static constexpr std::string_view cPitchDefault = "default";

	static constexpr std::string_view cRateXslow = "x-slow";
	static constexpr std::string_view cRateSlow = "slow";
	static constexpr std::string_view cRateMedium = "medium";
	static constexpr std::string_view cRateFast = "fast";
	static constexpr std::string_view cRateXfast = "x-fast";
	static constexpr std::string_view cRateDefault = "default";

	static constexpr std::string_view cVolumeSilent = "silent";
	static constexpr std::string_view cVolumeXsoft = "x-soft";
	static constexpr std::string_view cVolumeSoft = "soft";
	static constexpr std::string_view cVolumeMedium = "medium";
	static constexpr std::string_view cVolumeLoud = "loud";
	static constexpr std::string_view cVolumeXloud = "x-loud";
	static constexpr std::string_view cVolumeDefault = "default";
};
