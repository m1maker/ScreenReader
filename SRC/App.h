#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Object.h"
#include "Logger.h"
#include "Engine.h"
#include "Singleton.h"
#include "PlatformDependentWorker.h"
#include "PlatformDependentWorkerLinux.h"
#include "SpeechEngine.h"

inline bool g_running{false};
inline int g_retcode{0};

struct SScreenReaderOptions {
};

class CScreenReaderApp final {
	SScreenReaderOptions m_options;
	std::unique_ptr<IPlatformDependentWorker> m_worker;
public:
	CScreenReaderApp() {
		Run();
	}

	CScreenReaderApp(const std::vector<std::string>& args) {
		//...
		Run();
	}

	~CScreenReaderApp() {
		g_logger.Log(CLogger::DEBUG, "Application is shutting down now");
	}

	void Run() {
		auto state_speaker = g_speechEngine.GetSpeaker();
		state_speaker->Initialize();
		state_speaker->Speak("Screen reader on", true);
		m_worker = std::make_unique<CPlatformDependentWorkerLinux>();
		m_worker->Loop();
		state_speaker->Uninitialize();
	}
};

#define g_applicationInstance CSingleton<CScreenReaderApp>::GetInstance()

