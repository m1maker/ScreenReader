module;
#include <alsa/asoundlib.h>
#include <expected>
module Platforms.Linux.AudioEngine;

[[nodiscard]] static constexpr auto GetSndPcmFormatFromAudioFormat(EAudioFormat format) -> snd_pcm_format_t {
	using enum EAudioFormat;
	switch (format) {
	case U8:
		return SND_PCM_FORMAT_U8;
	case S16:
		return SND_PCM_FORMAT_S16;
	case S24:
		return SND_PCM_FORMAT_S24;
	case S32:
		return SND_PCM_FORMAT_S32;
	case F32:
		return SND_PCM_FORMAT_FLOAT;
	default:
		return SND_PCM_FORMAT_UNKNOWN;
	}
}

[[nodiscard]] static constexpr auto GetAudioEngineErrorFromAlsaError(int error) -> EAudioEngineError {
	using enum EAudioEngineError;
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
	default:
		return FAIL;
	}
}

CAudioEngineAlsa::~CAudioEngineAlsa() {
	Uninitialize();
}

[[nodiscard]] auto CAudioEngineAlsa::Initialize(SAudioParameters parameters) -> AudioEngineResult<> {
	if (m_handle)
		return AudioEngineResult<>();

	auto result = snd_pcm_open(&m_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (result < 0)
		return std::unexpected(GetAudioEngineErrorFromAlsaError(result));

	result = snd_pcm_set_params(m_handle,
		GetSndPcmFormatFromAudioFormat(parameters.format),
		SND_PCM_ACCESS_RW_INTERLEAVED,
		parameters.channels,
		parameters.sample_rate,
		0,
		parameters.period_size);
	if (result < 0)
		return std::unexpected(GetAudioEngineErrorFromAlsaError(result));

	return AudioEngineResult<>();
}

void CAudioEngineAlsa::Uninitialize() {
	if (!m_handle)
		return;

	snd_pcm_close(m_handle);
	m_handle = nullptr;
}

[[nodiscard]] auto CAudioEngineAlsa::Write(const signed short int* buffer, unsigned long long frames)
	-> AudioEngineResult<> {
	if (!m_handle)
		return std::unexpected(EAudioEngineError::DEFUNCT);

	else if (!buffer || frames <= 0) [[unlikely]] {
		return std::unexpected(EAudioEngineError::INVALID_ARGUMENTS);
	}

	auto result = snd_pcm_writei(m_handle, buffer, frames);
	if (result == -EPIPE) {
		snd_pcm_prepare(m_handle);
		result = snd_pcm_writei(m_handle, buffer, frames);
	}
	return result > 0 ? AudioEngineResult<>() : std::unexpected(GetAudioEngineErrorFromAlsaError(result));
}

void CAudioEngineAlsa::Wait() {
	if (!m_handle)
		return;

	snd_pcm_drain(m_handle);
}

void CAudioEngineAlsa::Drop() {
	if (!m_handle)
		return;

	snd_pcm_drop(m_handle);
}
