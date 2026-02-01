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
	DeclareSingleton(CLogger);
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
	ELogLevel m_level{DEBUG};
	std::string m_currentCategory{"Unknown"};
	std::mutex m_mutex;

	explicit CLogger()  {
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
			m_file << '\n';
			m_file.close();
		}
	}
public:

	[[nodiscard]] static constexpr inline auto LogLevelToString(const ELogLevel& level) -> std::string_view {
		switch (level) {
			case INFO: return "Info";
			case DEBUG: return "Debug";
			case WARNING: return "Warning";
			case ERROR: return "Error";
			default: return "Nothing";
		}
	}

	template<typename T>
	inline void Log(const ELogLevel& level, const std::string& category, const T& value) noexcept {
		if (m_level > level) return;

		[[maybe_unused]] std::scoped_lock _(m_mutex);
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
	inline auto GetLevel() const -> const ELogLevel& { return m_level; }

	inline void SetCurrentCategory(const std::string& category) { m_currentCategory = category; }
	inline auto GetCurrentCategory() const -> const std::string& { return m_currentCategory; }
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

	CScopedCategory(const std::string& category) : m_currentCategory(g_logger.GetCurrentCategory()) {
		
		g_logger.SetCurrentCategory(category);
	}

	~CScopedCategory() {
		g_logger.SetCurrentCategory(m_currentCategory);
	}
};

#if defined(__GNUC__) || defined(__clang__)
#define LogCalled() [[maybe_unused]] CScopedCategory _(std::string(__PRETTY_FUNCTION__))
#elif defined(_MSC_VER)
#define LogCalled() [[maybe_unused]] CScopedCategory _(std::string(__FUNCSIG__))
#else
#define LogCalled() [[maybe_unused]] CScopedCategory _(std::string(__PRETTY_func__))
#endif
