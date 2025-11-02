#pragma once
#include <fstream>
#include <string>
#include "Singleton.h"

class CLogger final {
public:

	enum ELogLevel : unsigned char {
		NOTHING = 0,
		INFO,
		WARNING,
		ERROR,
		DEBUG
	};

private:
	std::ofstream m_file;
	ELogLevel m_level;
	std::string m_currentCatigory{"Unknown"};
public:

	explicit CLogger() : m_level(INFO) {
		m_file.open("ScreenReader.log");
		Log(INFO, "Logger", "Initialized. Log level is " + LogLevelToString(m_level));
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
	inline void Log(const ELogLevel& level, const std::string& catigory, T value) {
		if (level < m_level) return;

		m_file << LogLevelToString(level) << ": [" << catigory << "] " << value << std::endl;
	}

	template<typename T>
	inline void Log(const ELogLevel& level, T value) {
		Log(level, m_currentCatigory, value);
	}

	inline void SetLevel(const ELogLevel& level) {
		m_level = level;
		Log(INFO, "Logger", "Log level is now " + LogLevelToString(m_level));
	}
	inline const ELogLevel& GetLevel() const { return m_level; }

	inline void SetCurrentCatigory(const std::string& catigory) { m_currentCatigory = catigory; }
	inline const std::string& GetCurrentCatigory() const { return m_currentCatigory; }
};

#define g_logger CSingleton<CLogger>::GetInstance()

class CScopedCatigory final {
	std::string m_currentCatigory;
public:

	CScopedCatigory(const std::string& catigory) {
		m_currentCatigory = g_logger.GetCurrentCatigory();
		g_logger.SetCurrentCatigory(catigory);
	}

	~CScopedCatigory() {
		g_logger.SetCurrentCatigory(m_currentCatigory);
	}
};

