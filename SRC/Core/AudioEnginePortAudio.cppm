module;
#include <portaudio.h>
export module Core.AudioEngine;
import Core.Audio;

export class CAudioEnginePortAudio final {
	PaStream* m_handle{nullptr};

public:
	explicit CAudioEnginePortAudio() = default;
	~CAudioEnginePortAudio();

	[[nodiscard]] auto Initialize(SAudioParameters parameters) -> AudioEngineResult<>;
	void Uninitialize();

	[[nodiscard]] auto Write(const signed short int* buffer, unsigned long long frames) -> AudioEngineResult<>;
	void Wait();
	void Drop();
};

export using BuiltInAudioEngine = CAudioEnginePortAudio;
