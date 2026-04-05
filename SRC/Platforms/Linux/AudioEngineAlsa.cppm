module;
#include <alsa/asoundlib.h>
export module Platforms.Linux.AudioEngine;
import Core.Audio;

export class CAudioEngineAlsa final {
	snd_pcm_t* m_handle{nullptr};

public:
	explicit CAudioEngineAlsa() = default;
	~CAudioEngineAlsa();

	[[nodiscard]] auto Initialize(SAudioParameters parameters) -> AudioEngineResult<>;
	void Uninitialize();

	[[nodiscard]] auto Write(const signed short int* buffer, unsigned long long frames) -> AudioEngineResult<>;
	void Wait();
	void Drop();
};
