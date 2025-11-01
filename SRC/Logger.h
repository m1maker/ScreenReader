#pragma once
#include <fstream>
#include <string>
#include "Singleton.h"

class CLogger final {
	std::ofstream m_file;
	int m_level;
public:

	enum ELogLevel : unsigned char {
		NOTHING = 0,
		INFO,
		DEBUG,
		WARNING,
		ERROR
	};

	explicit CLogger() : m_level(INFO) {
		m_file.open("ScreenReader.log");
	}

	~CLogger() = default;

	[[nodiscard]] static constexpr inline std::string LogLevelToString(const ELogLevel& level) {
		switch (level) {
			case INFO: return "Info";
			case DEBUG: return "Debug";
			case WARNING: return "Warning";
			case ERROR: return "Error";
			default: return "";
		}
	}

	template<typename T>
	void Log(const ELogLevel& level, T value) {
		if (level < m_level) return;

		m_file << LogLevelToString(level) << ": " << value << std::endl;
	}
};

#define g_logger CSingleton<CLogger>::GetInstance()

