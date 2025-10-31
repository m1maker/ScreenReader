#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Object.h"
#include "Engine.h"
#include "Singleton.h"
#include "PlatformDependentWorker.h"
#include "PlatformDependentWorkerLinux.h"

inline bool g_running{false};
inline int g_retcode{0};

struct SScreenReaderOptions {
};

class CScreenReaderApp final {
	SScreenReaderOptions m_options;
	std::unique_ptr<IPlatformDependentWorker> m_worker;
public:
	CScreenReaderApp() {
		Run();
	}

	CScreenReaderApp(const std::vector<std::string>& args) {
		//...
		Run();
	}

	~CScreenReaderApp() {
	}

	void Run() {
		m_worker = std::make_unique<CPlatformDependentWorkerLinux>();
		m_worker->Loop();
	}
};

#define g_applicationInstance CSingleton<CScreenReaderApp>::GetInstance()

