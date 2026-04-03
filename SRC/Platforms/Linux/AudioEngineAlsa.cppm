module;
export module Platforms.Linux.AudioEngine;

class CAudioEngineAlsa final {
	bool m_initialized{false};

public:
	explicit CAudioEngineAlsa() = default;
	CAudioEngineAlsa();
};
