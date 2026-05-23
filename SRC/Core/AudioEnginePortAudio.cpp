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

[[nodiscard]] static constexpr auto GetAudioEngineErrorFromPaError(PaError error) -> EAudioEngineError {
	using enum EAudioEngineError;
	/*
	switch (error) {
	case 0:
		return SUCCESS;
	case -ENODEV:
	case -ENXIO:
	case -ENOTTY:
	case -EBADF:
		return DEFUNCT;
	case -ENOSYS:
	case -ENOTSUP:
		return NOT_SUPPORTED;
	case -EACCES:
	case -EPERM:
		return ACCESS_DENIED;
	case -EINVAL:
	case -EFAULT:
		return INVALID_ARGUMENTS;
	case -EPIPE:
		return INVALID_FORMAT;
	case -ENOBUFS:
	case -ENOMEM:
		return BUFFER_FULL;
	case -EBUSY:
	case -EAGAIN:
		return DEVICE_UNAVAILABLE;
	case -ETIMEDOUT:
	case -ETIME:
		return TIMEOUT;
*/
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
