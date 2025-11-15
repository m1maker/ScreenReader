// Program entry point.
#include "App.h"
#include <exception>

signed int main(signed int argc, char** argv) {
	/*
	We log exceptions directly from main, not from CScreenReaderApp.

	We'll simply log anything we don't handle in other parts of the application.
	*/
	try {
		g_applicationInstance; // Call the application constructor.
	}
	catch (std::exception& standard_exception) {
		g_logger.Log(CLogger::ERROR, "Application", standard_exception.what());
	}

	return g_returnCode.ToInt();
}

