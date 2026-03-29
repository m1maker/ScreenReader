// Program entry point.
import Core.App;
import Core.AppState;
import Core.Logger;
import Core.SpeechSystem;

#include <Version.h>
#include <exception>

auto main(signed int argc, char** argv) -> signed int {
	Module<"main"> module;
	/*
	We log exceptions directly from main, not from ScreenReaderApp.

	We'll simply log anything we don't handle in other parts of the application.
	*/
	try {
		SpeechSystem::GetInstance().Speak("Screenreader");
		ScreenReaderApp::GetInstance().Run();
	}
	catch (const std::exception& standard_exception) {
		module.LogException(standard_exception);
	}

	return g_returnCode.ToInt();
}
