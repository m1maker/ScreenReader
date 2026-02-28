// Main application class for screen reader.
module;
#include "Environment.h"
#include "Logger.h"
#include "Sral.hpp"

#include <atomic>
#include <memory>
#include <string>
#include <vector>
#if SR_LINUX
#include <Platforms/Linux/PlatformDependentWorkerLinux.h>
#endif

#include <Version.h>
extern constinit int g_speechEngineIndex;
export module Core.App;
import Core.AppState;
import Core.Config;
import Core.EventHandler;

// Define different screen reader startup options, command line and configuration.
struct SScreenReaderAppOptions final {
	SScreenReaderAppSettings settings;
};

export class CScreenReaderApp final {
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
	static auto& GetInstance() {
		static CScreenReaderApp instance;
		return instance;
	}

	void Run() {
		g_logger.Log(CLogger::INFO,
			"Application",
			"Starting " + std::string(SScreenReaderVersion::PROJECT_NAME) + " version " +
				std::string(SScreenReaderVersion::STRING));
		g_running.store(true);
		g_speechEngineIndex = m_speechSystem.GetCurrentEngineId();
		CEventHandler::GetInstance().Start();
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
