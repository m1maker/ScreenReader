// Program entry point.
#include "App.h"
#include <exception>

auto main(signed int argc, char** argv) -> signed int {
	/*
	We log exceptions directly from main, not from CScreenReaderApp.

	We'll simply log anything we don't handle in other parts of the application.
	*/
	g_logger; // Invoke the logger instance to avoid uninitialization before the application instance.
	try {
		g_applicationInstance.Run();
	}
	catch (const std::exception& standard_exception) {
		g_logger.Log(CLogger::ERROR, standard_exception.what());
	}

	return g_returnCode.ToInt();
}

