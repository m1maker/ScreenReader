module;
#include <expected>
export module Core.Audio;

export using AudioSample = signed short int;

export enum class EAudioFormat : unsigned char { UNKNOWN = 0, U8, S16, S24, S32, F32 };

export template <EAudioFormat Format> struct TAudioFormatSize final {};
template <> struct TAudioFormatSize<EAudioFormat::U8> final {
	static constexpr unsigned char value = 1;
};
template <> struct TAudioFormatSize<EAudioFormat::S16> final {
	static constexpr unsigned char value = 2;
};
template <> struct TAudioFormatSize<EAudioFormat::S24> final {
	static constexpr unsigned char value = 3;
};
template <> struct TAudioFormatSize<EAudioFormat::S32> final {
	static constexpr unsigned char value = 4;
};
template <> struct TAudioFormatSize<EAudioFormat::F32> final {
	static constexpr unsigned char value = 4;
};
template <> struct TAudioFormatSize<EAudioFormat::UNKNOWN> final {};

export struct SAudioParameters final {
	unsigned char channels{0};
	unsigned int sample_rate{0};
	EAudioFormat format{EAudioFormat::UNKNOWN};
	unsigned long int period_size{0};

	[[nodiscard]] static constexpr auto GetDefault() -> SAudioParameters {
		return SAudioParameters{.channels = 1, .sample_rate = 22500, .format = EAudioFormat::S16, .period_size = 60};
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
