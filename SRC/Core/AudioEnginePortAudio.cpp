module;
#include <expected>
#include <mutex>
#include <portaudio.h>
module Core.AudioEngine;
import Core.AudioSystem;

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

	auto result = Pa_Initialize();
	if (result != paNoError) {
		return std::unexpected(GetAudioEngineErrorFromPaError(result));
	}

	result = Pa_OpenDefaultStream(&m_handle,
		0,
		parameters.channels,
		GetPaSampleFormatFromAudioFormat(parameters.format),
		parameters.sample_rate,
		parameters.period_size,
		&CAudioEnginePortAudio::PaCallback,
		this);
	if (result != paNoError) {
		return std::unexpected(GetAudioEngineErrorFromPaError(result));
	}

	result = Pa_StartStream(m_handle);
	if (result != paNoError) {
		return std::unexpected(GetAudioEngineErrorFromPaError(result));
	}
	return AudioEngineResult<>();
}

void CAudioEnginePortAudio::Uninitialize() {
	if (!m_handle)
		return;

	Pa_StopStream(m_handle);
	Pa_CloseStream(m_handle);
	m_handle = nullptr;
	Pa_Terminate();
}

int CAudioEnginePortAudio::PaCallback([[maybe_unused]] const void* pInput,
	void* pOutput,
	unsigned long int frames_per_buffer,
	[[maybe_unused]] const PaStreamCallbackTimeInfo* pTimeInfo,
	[[maybe_unused]] PaStreamCallbackFlags status_flags,
	void* pUserData) {
	auto engine = static_cast<CAudioEnginePortAudio*>(pUserData);
	if (!engine) [[unlikely]]
		return paContinue;
	return engine->AudioCallback(static_cast<signed short int*>(pOutput), frames_per_buffer);
}

[[nodiscard]] auto CAudioEnginePortAudio::AudioCallback(signed short int* buffer, unsigned long long int frames)
	-> int {
	std::scoped_lock _(m_callbackMutex);
	auto& audio_system = AudioSystem::GetInstance();
	audio_system.Read(buffer, frames);
	return paContinue;
}
