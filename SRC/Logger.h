#pragma once
#include <fstream>
#include <string>
#include "Singleton.h"

class CLogger final {
public:

	enum ELogLevel : unsigned char {
		NOTHING = 0,
		INFO,
		DEBUG,
		WARNING,
		ERROR
	};

private:
	std::ofstream m_file;
	ELogLevel m_level;
public:

	explicit CLogger() : m_level(INFO) {
		m_file.open("ScreenReader.log");
		Log(INFO, "Logger initialized. Log level is " + LogLevelToString(m_level));
	}

	~CLogger() = default;

	[[nodiscard]] static constexpr inline std::string LogLevelToString(const ELogLevel& level) {
		switch (level) {
			case INFO: return "Info";
			case DEBUG: return "Debug";
			case WARNING: return "Warning";
			case ERROR: return "Error";
			default: return "Nothing";
		}
	}

	template<typename T>
	inline void Log(const ELogLevel& level, T value) {
		if (level < m_level) return;

		m_file << LogLevelToString(level) << ": " << value << std::endl;
	}

	inline void SetLevel(const ELogLevel& level) {
		m_level = level;
		Log(INFO, "Log level is now " + LogLevelToString(m_level));
	}
};

#define g_logger CSingleton<CLogger>::GetInstance()

