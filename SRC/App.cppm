export module App;
#include "Object.h"
#include <string>
#include <vector>

export bool g_running{false};

struct SScreenReaderOptions {
};

export class CScreenReaderApp final
	SScreenReaderOptions m_options;
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
