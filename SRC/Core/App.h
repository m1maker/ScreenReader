// Main application class for screen reader.
#pragma once
#include "AppState.h"
#include "Environment.h"
#include "Logger.h"
#include "Singleton.h"

#include <Interfaces/Object.h>
#include <atomic>
#include <memory>
#include <string>
#include <vector>
#if SR_LINUX
#include <Platforms/Linux/PlatformDependentWorkerLinux.h>
#endif
#include "Config.h"
#include "EventHandler.h"
#include "SpeechEngine.h"

#include <Version.h>

// Define different screen reader startup options, command line and configuration.
struct SScreenReaderAppOptions final {
	SScreenReaderAppSettings settings;
};

class CScreenReaderApp final {
	DeclareSingleton(CScreenReaderApp);
	explicit CScreenReaderApp() = default;

	SScreenReaderAppOptions m_options;

	Sral::System m_speechSystem;
	/*
	A platform dependent worker is the screen reader's most basic and main loop.
	On Linux, it's an AT-SPI event loop; on Windows, I think it's some kind of GetMessage/DispatchMessage (windows-like
	loops).
	*/
	CPlatformDependentWorker m_worker;
	unsigned int m_loopRestartAttempts{0};

	~CScreenReaderApp() {
		g_logger.Log(CLogger::INFO,
			"Application",
			"Shutting down with return code " + std::to_string(g_returnCode.ToInt()) + " - " +
				std::string(g_returnCode.ToString()));
	}

public:
	void Run() {
		g_logger.Log(CLogger::INFO,
			"Application",
			"Starting " + std::string(SScreenReaderVersion::PROJECT_NAME) + " version " +
				std::string(SScreenReaderVersion::STRING));
		g_running.store(true);
		g_speechEngineIndex = m_speechSystem.GetCurrentEngineId();
		m_speechSystem.GetEngine(g_speechEngineIndex).Speak("Screen reader on");
		g_eventHandler.Start(); // It's the same as CSingleton<CEventHandler>::GetInstance()
		/*
		Don't terminate the application while g_running is true. This is the only flag that explicitly tells us to
		terminate the program. Even if the main loop terminates for some strange reason, we'll restart it.
		*/
		while (g_running.load()) {
			g_logger.Log(CLogger::DEBUG,
				"Application",
				"Entering worker loop. Attempt: " + std::to_string(m_loopRestartAttempts + 1));
			m_worker.Loop();
			++m_loopRestartAttempts;
			if (g_running)
				g_logger.Log(CLogger::WARNING, "Application", "Worker loop exited. Restarting");
		}

		g_running.store(false);
		g_logger.Log(CLogger::INFO, "Application", "Worker finished");
	}

	[[nodiscard]] auto GetSettings() -> SScreenReaderAppSettings& { return m_options.settings; }

	[[nodiscard]] inline auto GetSpeechSystem() -> Sral::System& { return m_speechSystem; }
};

#define g_applicationInstance CSingleton<CScreenReaderApp>::GetInstance()
