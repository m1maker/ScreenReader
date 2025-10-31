#include "SpeechEngine.h"

CSpeechEngine::CSpeechEngine() {
	m_speechEngine = std::make_shared<Sral::SpeechDispatcher>();
	}

