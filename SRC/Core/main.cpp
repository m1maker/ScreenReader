// Program entry point.
import Core.App;
import Core.AppState;
#include "Logger.h"

#include <exception>

constinit int g_speechEngineIndex = 0;

auto main(signed int argc, char** argv) -> signed int {
	/*
	We log exceptions directly from main, not from CScreenReaderApp.

	We'll simply log anything we don't handle in other parts of the application.
	*/
	g_logger; // Invoke the logger instance to avoid uninitialization before the application instance.
	try {
		CScreenReaderApp::GetInstance().Run();
	}
	catch (const std::exception& standard_exception) {
		g_logger.Log(CLogger::ERROR, standard_exception.what());
	}

	return g_returnCode.ToInt();
}
