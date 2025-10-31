#pragma once
#include <memory>
#include "Singleton.h"
#include "Engine.h"
#include "EngineSpeechDispatcher.h"

class CSpeechEngine final {
	std::shared_ptr<Sral::Engine> m_speechEngine;
public:
	CSpeechEngine();
	~CSpeechEngine() = default;

	[[nodiscard]] inline std::shared_ptr<Sral::Engine> GetSpeaker() {
		return m_speechEngine;
	}
};

#define g_speechEngine CSingleton<CSpeechEngine>::GetInstance()

