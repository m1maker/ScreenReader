module;
#include "ScopedPool.h"

#include <map>
#include <memory>
#include <memory_resource>
#include <string>
#include <string_view>
export module Core.SsmlUtterance;

export class CSsmlUtterance final {
	static constexpr const size_t cBufferSize = 16384;
	alignas(64) char __buf_[cBufferSize];
	std::pmr::monotonic_buffer_resource m_pool;

	size_t m_prefixLength{0};
	std::pmr::string m_ssmlContent;
	std::string_view m_currentPitch;
	std::string_view m_currentRate;
	std::string_view m_currentVolume;
	std::string_view m_currentVoice;
	bool m_inProsody{false};

	void StartProsodyIfNeeded();
	void EndProsodyIfNeeded();
	[[nodiscard]] auto EscapeXml(std::string_view text) -> std::pmr::string;

public:
	explicit CSsmlUtterance();

	auto Text(this auto&& self, std::string_view text);
	auto Break(this auto&& self, std::string_view time = "500ms");

	auto Mark(this auto&& self, std::string_view name);

	auto Pitch(this auto&& self, std::string_view pitch);
	auto Rate(this auto&& self, std::string_view rate);
	auto Volume(this auto&& self, std::string_view volume);

	auto Voice(this auto&& self, std::string_view voice);

	[[nodiscard]] auto ToSsml() -> std::string_view;
	[[nodiscard]] constexpr operator std::string_view() { return ToSsml(); }

	void Clear();

	static constexpr const std::string_view cPitchXlow = "x-low";
	static constexpr const std::string_view cPitchLow = "low";
	static constexpr const std::string_view cPitchMedium = "medium";
	static constexpr const std::string_view cPitchHigh = "high";
	static constexpr const std::string_view cPitchXhigh = "x-high";
	static constexpr const std::string_view cPitchDefault = "default";

	static constexpr const std::string_view cRateXslow = "x-slow";
	static constexpr const std::string_view cRateSlow = "slow";
	static constexpr const std::string_view cRateMedium = "medium";
	static constexpr const std::string_view cRateFast = "fast";
	static constexpr const std::string_view cRateXfast = "x-fast";
	static constexpr const std::string_view cRateDefault = "default";

	static constexpr const std::string_view cVolumeSilent = "silent";
	static constexpr const std::string_view cVolumeXsoft = "x-soft";
	static constexpr const std::string_view cVolumeSoft = "soft";
	static constexpr const std::string_view cVolumeMedium = "medium";
	static constexpr const std::string_view cVolumeLoud = "loud";
	static constexpr const std::string_view cVolumeXloud = "x-loud";
	static constexpr const std::string_view cVolumeDefault = "default";
};
