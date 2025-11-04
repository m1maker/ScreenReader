// Main application class for screen reader.
#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Object.h"
#include "Logger.h"
#include "Engine.h"
#include "Singleton.h"
#include "PlatformDependentWorker.h"
#include "PlatformDependentWorkerLinux.h"
#include "SpeechEngine.h"
#include "EventHandler.h"
#include "EventToSpeech.h"

/*
If g_running is false, all processes, loops, and threads should exit.
After that, application must return g_retcode.

However, a screen reader is a program that must be robust against exceptions and continue running.
Hopefully, these flags will only be used when we disable the screen reader.
*/
inline bool g_running{false};
inline int g_retcode{0};

// Define different screen reader startup options, command line and configuration.
struct SScreenReaderOptions {
	// Now empty
};

class CScreenReaderApp final {
	SScreenReaderOptions m_options;
	/*
	A platform dependent worker is the screen reader's most basic and main loop.
	On Linux, it's an AT-SPI event loop; on Windows, I think it's some kind of GetMessage/DispatchMessage (windows-like loops).
	*/
	std::unique_ptr<IPlatformDependentWorker> m_worker;
public:
	CScreenReaderApp() { // Without command line options
		Run();
	}

	CScreenReaderApp(const std::vector<std::string>& args) {
		//...ParseParseParse
		Run();
	}

	~CScreenReaderApp() {
		g_logger.Log(CLogger::INFO, "Application", "Shutting down");
	}

	void Run() {
		g_logger.Log(CLogger::INFO, "Application", "Starting");
		/*
		We get a speaker for announcing states.
		We get it from several places, so it's a shared_ptr.

		When the screen reader's voice changes, it will change everywhere; for now, there's only one speaker.
		Perhaps in the future, I'll implement different voices for attributes.
		*/
		auto state_speaker = g_speechEngine.GetSpeaker();
		state_speaker->Initialize(); // Here initializing
		state_speaker->Speak("Screen reader on", true);
		g_eventHandler; // It's the same as CSingleton<CEventHandler>::GetInstance()
		m_worker = std::make_unique<CPlatformDependentWorkerLinux>(); // In the future, this will of course be platform specific.
		g_eventToSpeech.AnnounceWhereAmI();
		m_worker->Loop(); // Start the main loop.
		state_speaker->Uninitialize();
		g_logger.Log(CLogger::INFO, "Application", "Worker finished");
	}
};

#define g_applicationInstance CSingleton<CScreenReaderApp>::GetInstance()

