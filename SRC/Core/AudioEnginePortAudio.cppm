module;
#include <mutex>
#include <portaudio.h>
export module Core.AudioEngine;
import Core.Audio;

export class CAudioEnginePortAudio final {
	PaStream* m_handle{nullptr};

	std::mutex m_callbackMutex;

	static int PaCallback([[maybe_unused]] const void* pInput,
		void* pOutput,
		unsigned long int frames_per_buffer,
		[[maybe_unused]] const PaStreamCallbackTimeInfo* pTimeInfo,
		[[maybe_unused]] PaStreamCallbackFlags status_flags,
		void* pUserData);

	[[nodiscard]] auto AudioCallback(signed short int* buffer, unsigned long long int frames) -> int;

public:
	~CAudioEnginePortAudio();

	[[nodiscard]] auto Initialize(SAudioParameters parameters) -> AudioEngineResult<>;
	void Uninitialize();
};

export using BuiltInAudioEngine = CAudioEnginePortAudio;
