/*
* This file is part of the Screen Reader project.
*
* Copyright (C) 2025-2026  Georgiy Bondarenko (M_maker / m1maker) <georgijbondarenko248@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

// Main application class for screen reader.
module;
#include "Environment.h"

#include <atomic>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
export module Core.App;
import Core.AppState;
import Core.AudioSystem;
import Core.Config;
import Core.Environment;
import Core.EventHandler;
import Core.Logger;
import Core.Singleton;
import Core.SpeechSystem;

// Define different screen reader startup options, command line and configuration.
struct SScreenReaderAppOptions final {
	SScreenReaderAppSettings settings;
};

export class ScreenReaderApp final : TModule<"Application">, public TSingleton<ScreenReaderApp> {

	SScreenReaderAppOptions m_options;

	/*
	A platform dependent worker is the screen reader's most basic and main loop.
	On Linux, it's an AT-SPI event loop; on Windows, I think it's some kind of GetMessage/DispatchMessage (windows-like
	loops).
	*/
	DefaultPlatformDependentWorker m_worker;
	unsigned int m_loopRestartAttempts{0};

public:
	void Run() {
		g_running.store(true);
		/*
		We are trying to change the working directory to the executable directory, because we need to read some data,
		such as built-in espeak voice/language data.
		*/
		try {
			auto executable_path = m_worker.GetExecutablePath();
			auto working_directory = executable_path.parent_path();
			Log(DEBUG, "Switching directory to {}", working_directory.string());
			std::filesystem::current_path(working_directory);
		}
		catch (const std::filesystem::filesystem_error& filesystem_error) {
			LogException(filesystem_error);
			g_running.store(false);
		}

		SpeechSystem::GetInstance().Speak("ScreenReader");

		EventHandler::GetInstance().Start();
		AudioSystem::GetInstance().Start();
		SpeechSystem::GetInstance().Start();
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
