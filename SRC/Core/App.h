// Main application class for screen reader.
#pragma once
#include "Environment.h"
#include "AppState.h"
#include <memory>
#include <vector>
#include <string>
#include <atomic>
#include <Interfaces/Object.h>
#include "Logger.h"
#include "Singleton.h"
#include <Interfaces/PlatformDependentWorker.h>
#if SR_LINUX
#include <Platforms/Linux/PlatformDependentWorkerLinux.h>
#endif
#include "SpeechEngine.h"
#include "EventHandler.h"
#include <Version.h>
#include "Config.h"

// Define different screen reader startup options, command line and configuration.
struct SScreenReaderAppOptions final {
	SScreenReaderAppSettings settings;
};

class CScreenReaderApp final {
	DeclareSingleton(CScreenReaderApp);
	explicit CScreenReaderApp() {
	}

	SScreenReaderAppOptions m_options;

	Sral::System m_speechSystem;
	/*
	A platform dependent worker is the screen reader's most basic and main loop.
	On Linux, it's an AT-SPI event loop; on Windows, I think it's some kind of GetMessage/DispatchMessage (windows-like loops).
	*/
	std::unique_ptr<IPlatformDependentWorker> m_worker;
	unsigned int m_loopRestartAttempts{0};

	~CScreenReaderApp() {
		g_logger.Log(CLogger::INFO, "Application", "Shutting down with return code " + std::to_string(g_returnCode.ToInt()) + " - " + std::string(g_returnCode.ToString()));
	}

public:
	void Run() {
		g_logger.Log(CLogger::INFO, "Application", "Starting " + std::string(SScreenReaderVersion::PROJECT_NAME) + " version " + std::string(SScreenReaderVersion::STRING));
		g_running.store(true);
		g_speechEngineIndex = m_speechSystem.GetCurrentEngineId();
		m_speechSystem.GetEngine(g_speechEngineIndex).Speak("Screen reader on");
		g_eventHandler; // It's the same as CSingleton<CEventHandler>::GetInstance()
#if SR_LINUX
		m_worker = std::make_unique<CPlatformDependentWorkerLinux>();
#else
		g_returnCode = CScreenReaderAppReturnCode::ERROR_NOT_SUPPORTED;
		return;
#endif
		/*
		Don't terminate the application while g_running is true. This is the only flag that explicitly tells us to terminate the program.
		Even if the main loop terminates for some strange reason, we'll restart it.
		*/
		while (g_running.load()) {
			g_logger.Log(CLogger::DEBUG, "Application", "Entering worker loop. Attempt: " + std::to_string(m_loopRestartAttempts + 1));
			m_worker->Loop();
			++m_loopRestartAttempts;
			if (g_running) g_logger.Log(CLogger::WARNING, "Application", "Worker loop exited. Restarting");
		}

		g_running.store(false);
		g_logger.Log(CLogger::INFO, "Application", "Worker finished");
	}

	[[nodiscard]] auto GetSettings() -> SScreenReaderAppSettings& {
		return m_options.settings;
	}

	[[nodiscard]] inline auto GetSpeechSystem() -> Sral::System& {
		return m_speechSystem;
	}
};

#define g_applicationInstance CSingleton<CScreenReaderApp>::GetInstance()
