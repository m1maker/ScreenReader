// Program entry point.
import Core.App;
import Core.AppState;
import Core.Logger;

#include <Version.h>
#include <exception>

auto main(signed int argc, char** argv) -> signed int {
	TModule<"main"> module;
	/*
	We log exceptions directly from main, not from ScreenReaderApp.

	We'll simply log anything we don't handle in other parts of the application.
	*/
	try {
		ScreenReaderApp::GetInstance().Run();
	}
	catch (const std::exception& standard_exception) {
		module.LogException(standard_exception);
	}

	return g_returnCode.ToInt();
}
