#pragma once
#include "Sral.hpp"

inline constinit int g_speechEngineIndex = SRAL_ENGINE_NONE;
static constexpr inline int cSralEnginesExclude = 
	SRAL_ENGINE_NVDA | 
	SRAL_ENGINE_JAWS | 
	SRAL_ENGINE_ZDSR | 
	SRAL_ENGINE_NARRATOR | 
	SRAL_ENGINE_UIA | 
	SRAL_ENGINE_VOICE_OVER
;

#define g_speechSystem g_applicationInstance.GetSpeechSystem()
#define g_speechEngine g_speechSystem.GetEngine(g_speechEngineIndex)
