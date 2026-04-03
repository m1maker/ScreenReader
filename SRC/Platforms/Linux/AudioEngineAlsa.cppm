module;
#include <alsa/asoundlib.h>
export module Platforms.Linux.AudioEngine;
import Core.Audio;

class CAudioEngineAlsa final {
	snd_pcm_t* m_handle{nullptr};
	SAudioParameters m_parameters;
public:
	explicit CAudioEngineAlsa() = default;
	~CAudioEngineAlsa();

	[[nodiscard]] auto Initialize(SAudioParameters parameters) -> AudioEngineResult<>;
	void Uninitialize();

	[[nodiscard]] auto Write(const void* buffer, unsigned long long frames) -> AudioEngineResult<>;
};
