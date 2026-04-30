module;
#include <expected>
#include <string_view>
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
	DEVICE_UNAVAILABLE,
	TIMEOUT,
	FAIL
};

export [[nodiscard]] constexpr auto AudioEngineErrorToString(EAudioEngineError error) -> std::string_view {
	using enum EAudioEngineError;
	switch (error) {
	case SUCCESS:
		return "No error: The audio operation completed successfully.";
	case DEFUNCT:
		return "Audio Engine Defunct: The audio engine instance is no longer valid or has been destroyed.";
	case NOT_SUPPORTED:
		return "Operation Not Supported: The requested audio operation is not supported by this audio engine or "
			   "device.";
	case ACCESS_DENIED:
		return "Access Denied: Insufficient permissions to access the audio device or engine resources.";
	case INVALID_ARGUMENTS:
		return "Invalid Arguments: One or more parameters provided to the audio method are invalid or out of range.";
	case INVALID_FORMAT:
		return "Invalid Audio Format: The specified audio format is not supported or malformed for this operation.";
	case BUFFER_FULL:
		return "Buffer Full: The audio buffer has reached its maximum capacity and cannot accept more data.";
	case DEVICE_UNAVAILABLE:
		return "Device Unavailable: The requested audio device is not available, disconnected, or in use by another "
			   "application.";
	case TIMEOUT:
		return "Operation Timeout: The audio operation exceeded the maximum allowed time without completing.";
	case FAIL:
	default:
		return "Unknown Error: An unexpected or unspecified failure occurred during the audio engine operation.";
	}
}

export template <typename T = void> using AudioEngineResult = std::expected<T, EAudioEngineError>;
