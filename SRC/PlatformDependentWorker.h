// Platform dependent worker interface.
#pragma once

/*
A platform dependent worker is the screen reader's most basic and main loop.
*/

class IPlatformDependentWorker {
public:

	IPlatformDependentWorker() = default;
	virtual ~IPlatformDependentWorker() = default;

	virtual void Loop() = 0;
};

