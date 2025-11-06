// Platform dependent worker interface.
#pragma once

/*
A platform dependent worker is the screen reader's most basic and main loop.
*/
class IPlatformDependentWorker {
public:

	IPlatformDependentWorker() = default;
	virtual ~IPlatformDependentWorker() = default;

	/*
	The Loop function should run as long as g_running is true.
	*/
	virtual void Loop() = 0;
};

