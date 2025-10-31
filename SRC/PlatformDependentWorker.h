#pragma once

class IPlatformDependentWorker {
public:

	IPlatformDependentWorker() = default;
	virtual ~IPlatformDependentWorker() = default;

	virtual void Loop() = 0;
};

