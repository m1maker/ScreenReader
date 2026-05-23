module;
#include <expected>
#include <portaudio.h>
module Core.AudioEngine;

[[nodiscard]] static constexpr auto GetPaSampleFormatFromAudioFormat(EAudioFormat format) -> PaSampleFormat {
	using enum EAudioFormat;
	switch (format) {
	case U8:
		return paUInt8;
	case S16:
		return paInt16;
	case S24:
		return paInt24;
	case S32:
		return paInt32;
	case F32:
		return paFloat32;
	case UNKNOWN:
		return 0;
	}
}

[[nodiscard]] static constexpr auto GetAudioEngineErrorFromPaError(PaErrorCode error) -> EAudioEngineError {
	using enum EAudioEngineError;
	switch (error) {
	case paNoError:
		return SUCCESS;
	case paNotInitialized:
	case paUnanticipatedHostError:
	case paInternalError:
		return DEFUNCT;
	case paSampleFormatNotSupported:
	case paIncompatibleHostApiSpecificStreamInfo:
	case paIncompatibleStreamHostApi:
	case paHostApiNotFound:
	case paInvalidHostApi:
		return NOT_SUPPORTED;
	case paInvalidDevice:
	case paDeviceUnavailable:
	case paBadIODeviceCombination:
		return DEVICE_UNAVAILABLE;
	case paInvalidChannelCount:
	case paInvalidSampleRate:
	case paInvalidFlag:
	case paBadStreamPtr:
	case paBadBufferPtr:
	case paNullCallback:
	case paStreamIsStopped:
	case paStreamIsNotStopped:
	case paCanNotReadFromACallbackStream:
	case paCanNotWriteToACallbackStream:
	case paCanNotReadFromAnOutputOnlyStream:
	case paCanNotWriteToAnInputOnlyStream:
		return INVALID_ARGUMENTS;
	case paBufferTooBig:
	case paBufferTooSmall:
		return BUFFER_FULL;
	case paTimedOut:
		return TIMEOUT;
	case paInputOverflowed:
	case paOutputUnderflowed:
	case paInsufficientMemory:
		return FAIL;
	}
	return FAIL;
}

CAudioEnginePortAudio::~CAudioEnginePortAudio() {
	Uninitialize();
}

[[nodiscard]] auto CAudioEnginePortAudio::Initialize(SAudioParameters parameters) -> AudioEngineResult<> {
	if (m_handle)
		return AudioEngineResult<>();

	return AudioEngineResult<>();
}

void CAudioEnginePortAudio::Uninitialize() {
	if (!m_handle)
		return;

	Pa_StopStream(m_handle);
	Pa_CloseStream(m_handle);
	m_handle = nullptr;
}

[[nodiscard]] auto CAudioEnginePortAudio::Write(const signed short int* buffer, unsigned long long frames)
	-> AudioEngineResult<> {
	if (!m_handle)
		return std::unexpected(EAudioEngineError::DEFUNCT);

	else if (!buffer || frames <= 0) [[unlikely]] {
		return std::unexpected(EAudioEngineError::INVALID_ARGUMENTS);
	}
	return AudioEngineResult<>();
}

void CAudioEnginePortAudio::Wait() {
	if (!m_handle)
		return;
}

void CAudioEnginePortAudio::Drop() {
	if (!m_handle)
		return;
}
