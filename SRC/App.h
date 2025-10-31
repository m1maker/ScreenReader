#pragma once
#include <vector>
#include <memory>
#include <string>
#include "Object.h"
#include "Engine.h"

inline bool g_running{false};
inline int g_retcode{0};

struct SScreenReaderOptions {
};

class CScreenReaderApp final {
	SScreenReaderOptions m_options;
	std::unique_ptr<Sral::Engine> m_speechEngine;
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
		
	}
};

