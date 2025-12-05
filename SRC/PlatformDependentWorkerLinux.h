// Platform dependent worker implementation for Linux.
#pragma once
#include <atspi/atspi.h>
#include <csignal>
#include <unistd.h>
#include "AppState.h"
#include "PlatformDependentWorker.h"

/*
We will also handle signals here to ensure safe exit.
*/
class CPlatformDependentWorkerLinux final : public IPlatformDependentWorker {
private:
	struct sigaction m_signalAction{};
public:

	static void HandleSignal(int signal) {
		switch (signal) {
			case SIGINT:
			case SIGTERM:
				g_running.store(false);
				break;
			default: break;
		}
	}

	explicit CPlatformDependentWorkerLinux() {
		memset(&m_signalAction, 0, sizeof(m_signalAction));
		m_signalAction.sa_handler = &HandleSignal;
		sigemptyset(&m_signalAction.sa_mask);
		m_signalAction.sa_flags = 0;

		sigaction(SIGINT, &m_signalAction, nullptr);
		sigaction(SIGTERM, &m_signalAction, nullptr);

		atspi_init();
	}

	~CPlatformDependentWorkerLinux() override {
		atspi_exit();
	}

	void Loop() override {
		atspi_event_main();
	}
};

