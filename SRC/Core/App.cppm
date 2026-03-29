// Main application class for screen reader.
module;
#include "Environment.h"

#include <atomic>
#include <memory>
#include <string>
#include <vector>
export module Core.App;
import Core.AppState;
import Core.Config;
import Core.EventHandler;
import Core.Logger;
#if SR_LINUX
import Platforms.Linux.Worker;
#endif

// Define different screen reader startup options, command line and configuration.
struct SScreenReaderAppOptions final {
	SScreenReaderAppSettings settings;
};

export class ScreenReaderApp final : Module<"Application"> {
	explicit ScreenReaderApp() = default;

	SScreenReaderAppOptions m_options;

	/*
	A platform dependent worker is the screen reader's most basic and main loop.
	On Linux, it's an AT-SPI event loop; on Windows, I think it's some kind of GetMessage/DispatchMessage (windows-like
	loops).
	*/
	CPlatformDependentWorker m_worker;
	unsigned int m_loopRestartAttempts{0};

	~ScreenReaderApp() {}

public:
	static auto& GetInstance() {
		static ScreenReaderApp instance;
		return instance;
	}

	void Run() {
		g_running.store(true);
		EventHandler::GetInstance().Start();
		/*
		Don't terminate the application while g_running is true. This is the only flag that explicitly tells us to
		terminate the program. Even if the main loop terminates for some strange reason, we'll restart it.
		*/
		while (g_running.load()) {
			Log(DEBUG, "Main loop running. Attempt: {}", m_loopRestartAttempts + 1);
			m_worker.Loop();
			++m_loopRestartAttempts;
		}

		g_running.store(false);
	}

	[[nodiscard]] auto GetSettings() -> SScreenReaderAppSettings& { return m_options.settings; }
};
