// Logger
#pragma once
#include "Singleton.h"

#include <fstream>
#include <mutex>
#include <string>
#include <string_view>
import Core.AppState;

// This is the simplest inline logger with levels and categories.
class CLogger final {
	DeclareSingleton(CLogger);

public:
	enum ELogLevel : unsigned char { DEBUG = 0, INFO, WARNING, ERROR, NOTHING };

private:
	std::ofstream m_file;
	ELogLevel m_level{DEBUG};
	std::string_view m_currentCategory{"Unknown"};
	std::mutex m_mutex;

	explicit CLogger() {
		m_file.open("ScreenReader.log", std::ios::app);
		if (!m_file.is_open()) {
			g_returnCode = CScreenReaderAppReturnCode::ERROR_LOGGER;
			g_running.store(false);
			return;
		}

		Log(INFO, "Logger", "Initialized. Log level is ", LogLevelToString(m_level));
	}

	~CLogger() {
		if (m_file.is_open()) {
			m_file << '\n';
			m_file.close();
		}
	}

public:
	[[nodiscard]] static constexpr inline auto LogLevelToString(ELogLevel level) -> std::string_view {
		switch (level) {
		case INFO:
			return "Info";
		case DEBUG:
			return "Debug";
		case WARNING:
			return "Warning";
		case ERROR:
			return "Error";
		default:
			return "Nothing";
		}
	}

	template <typename... Args> inline void Log(ELogLevel level, std::string_view category, Args&&... args) noexcept {
		if (m_level > level)
			return;

		[[maybe_unused]] std::scoped_lock _(m_mutex);
		m_file << LogLevelToString(level) << ": [" << category << "] ";
		((m_file << std::forward<Args>(args)), ...);
		m_file << '\n';
	}

	template <typename... Args>
		requires(sizeof...(Args) > 0)
	inline void Log(ELogLevel level, Args&&... args) noexcept
		requires(!std::is_convertible_v<std::tuple_element_t<0, std::tuple<Args...>>, std::string_view>)
	{
		Log(level, m_currentCategory, std::forward<Args>(args)...);
	}

	inline void SetLevel(ELogLevel level) noexcept {
		m_level = level;
		Log(INFO, "Logger", "Log level is now ", LogLevelToString(m_level));
	}

	inline auto GetLevel() const noexcept -> ELogLevel { return m_level; }

	inline void SetCurrentCategory(std::string_view category) { m_currentCategory = category; }
	inline auto GetCurrentCategory() const -> std::string_view { return m_currentCategory; }
};

#define g_logger CSingleton<CLogger>::GetInstance() // Global instance.

/*
A scoped category is an object that keeps one category active in the logger during its lifetime.
When we call the overloaded CLogger::Log without a category, it uses m_currentCategory.
This is useful if we use the logger many times in a single function. Then we set the category.

If a function that uses CScopedCategory calls a function that also uses the logger, we need to make sure that it also
forces its category or also uses CScopedCategory.
*/
class CScopedCategory final {
	std::string_view m_currentCategory;

public:
	CScopedCategory(std::string_view category) : m_currentCategory(g_logger.GetCurrentCategory()) {

		g_logger.SetCurrentCategory(category);
	}

	~CScopedCategory() { g_logger.SetCurrentCategory(m_currentCategory); }
};

#if defined(__GNUC__) || defined(__clang__)
#define LogCalled() [[maybe_unused]] CScopedCategory _(std::string_view(__PRETTY_FUNCTION__))
#elif defined(_MSC_VER)
#define LogCalled() [[maybe_unused]] CScopedCategory _(std::string_view(__FUNCSIG__))
#else
#define LogCalled() [[maybe_unused]] CScopedCategory _(std::string_view(__PRETTY_func__))
#endif
