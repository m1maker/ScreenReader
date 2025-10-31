#pragma once
#include <atspi/atspi.h>
#include "PlatformDependentWorker.h"

class CPlatformDependentWorkerLinux final : public IPlatformDependentWorker {
public:

	explicit CPlatformDependentWorkerLinux() {
		atspi_init();
	}

	~CPlatformDependentWorkerLinux() override {
		atspi_event_quit();
	}

	void Loop() override {
		atspi_event_main();
	}
};

