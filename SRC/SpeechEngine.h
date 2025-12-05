#pragma once
#include <memory>
#include "Singleton.h"
#include "Engine.h"
#include "EngineSpeechDispatcher.h"

class CSpeechEngine final {
	DeclareSingleton(CSpeechEngine);
	std::shared_ptr<Sral::Engine> m_speechEngine;
	explicit CSpeechEngine();
	~CSpeechEngine() = default;
public:

	[[nodiscard]] inline auto GetSpeaker() -> std::shared_ptr<Sral::Engine> {
		return m_speechEngine;
	}
};

#define g_speechEngine CSingleton<CSpeechEngine>::GetInstance()

