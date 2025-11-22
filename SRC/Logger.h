// Logger
#pragma once
#include <fstream>
#include <string>
#include "Singleton.h"
#include <string_view>
#include <mutex>
#include "AppState.h"

// This is the simplest inline logger with levels and categories.
class CLogger final {
public:

	enum ELogLevel : unsigned char {
		DEBUG = 0,
		INFO,
		WARNING,
		ERROR,
		NOTHING
	};

private:
	std::ofstream m_file;
	ELogLevel m_level;
	std::string m_currentCategory{"Unknown"};
	std::mutex m_mutex;
public:

	explicit CLogger() : m_level(DEBUG) {
		m_file.open("ScreenReader.log", std::ios::app);
		if (!m_file.is_open()) {
			g_returnCode = CScreenReaderAppReturnCode::ERROR_LOGGER;
			g_running.store(false);
			return;
		}

		Log(INFO, "Logger", "Initialized. Log level is " + std::string(LogLevelToString(m_level)));
	}

	~CLogger() {
		if (m_file.is_open()) {
			m_file << std::endl;
			m_file.close();
		}
	}

	[[nodiscard]] static constexpr inline std::string_view LogLevelToString(const ELogLevel& level) {
		switch (level) {
			case INFO: return "Info";
			case DEBUG: return "Debug";
			case WARNING: return "Warning";
			case ERROR: return "Error";
			default: return "Nothing";
		}
	}

	template<typename T>
	inline void Log(const ELogLevel& level, const std::string& category, T value) noexcept {
		if (m_level > level) return;

		[[maybe_unused]] std::lock_guard _(m_mutex);
		m_file << LogLevelToString(level) << ": [" << category << "] " << value << std::endl;
	}

	template<typename T>
	inline void Log(const ELogLevel& level, T value) noexcept {
		Log(level, m_currentCategory, value);
	}

	inline void SetLevel(const ELogLevel& level) {
		m_level = level;
		Log(INFO, "Logger", "Log level is now " + std::string(LogLevelToString(m_level)));
	}
	inline const ELogLevel& GetLevel() const { return m_level; }

	inline void SetCurrentCategory(const std::string& category) { m_currentCategory = category; }
	inline const std::string& GetCurrentCategory() const { return m_currentCategory; }
};

#define g_logger CSingleton<CLogger>::GetInstance() // Global instance.

/*
A scoped category is an object that keeps one category active in the logger during its lifetime.
When we call the overloaded CLogger::Log without a category, it uses m_currentCategory.
This is useful if we use the logger many times in a single function. Then we set the category.

If a function that uses CScopedCategory calls a function that also uses the logger, we need to make sure that it also forces its category or also uses CScopedCategory.
*/
class CScopedCategory final {
	std::string m_currentCategory;
public:

	CScopedCategory(const std::string& category) {
		m_currentCategory = g_logger.GetCurrentCategory();
		g_logger.SetCurrentCategory(category);
	}

	~CScopedCategory() {
		g_logger.SetCurrentCategory(m_currentCategory);
	}
};

