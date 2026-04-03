module;
#include <expected>
export module Core.Audio;

export enum class EAudioFormat : unsigned char { UNKNOWN = 0, U8, S16, S24, S32, F32 };

export struct SAudioParameters final {
	unsigned char channels{0};
	unsigned int sample_rate{0};
	EAudioFormat format{EAudioFormat::UNKNOWN};
	unsigned long int period_size{0};

	[[nodiscard]] static constexpr auto GetDefault() -> SAudioParameters {
		return SAudioParameters{.channels = 1, .sample_rate = 44100, .format = EAudioFormat::S16, .period_size = 60};
	}
};

export enum class EAudioEngineError : unsigned char {
	SUCCESS = 0,
	DEFUNCT,
	NOT_SUPPORTED,
	ACCESS_DENIED,
	INVALID_ARGUMENTS,
	INVALID_FORMAT,
	BUFFER_FULL,
	TIMEOUT,
	FAIL
};

export template <typename T = void> using AudioEngineResult = std::expected<T, EAudioEngineError>;
