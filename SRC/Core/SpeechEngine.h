#pragma once
#include "Sral.hpp"
import Core.App;

extern constinit int g_speechEngineIndex;
static constexpr inline int cSralEnginesExclude = SRAL_ENGINE_NVDA | SRAL_ENGINE_JAWS | SRAL_ENGINE_ZDSR |
	SRAL_ENGINE_NARRATOR | SRAL_ENGINE_UIA | SRAL_ENGINE_VOICE_OVER;

#define g_speechSystem CScreenReaderApp::GetInstance().GetSpeechSystem()
#define g_speechEngine g_speechSystem.GetEngine(g_speechEngineIndex)
