module;
#include <format>
#include <fstream>
#include <mutex>
#include <string>
#include <string_view>
export module Core.Logger;
import Core.AppState;

export enum class ELogLevel : unsigned char { DEBUG = 0, INFO, WARNING, ERROR, NOTHING };

// This is the simplest inline logger with levels and categories.
export class Logger final {
	using enum ELogLevel;
	std::ofstream m_file;
	ELogLevel m_level{DEBUG};
	std::mutex m_mutex;

	explicit Logger() {
		m_file.open("ScreenReader.log", std::ios::app);
		if (!m_file.is_open()) {
			g_returnCode = ScreenReaderAppReturnCode::ERROR_LOGGER;
			g_running.store(false);
			return;
		}

		Log(INFO, "Logger", "Initialized. Log level is {}", LogLevelToString(m_level));
	}

	~Logger() {
		if (m_file.is_open()) {
			m_file << '\n';
			m_file.close();
		}
	}

public:
	static auto& GetInstance() {
		static Logger instance;
		return instance;
	}

	[[nodiscard]] static constexpr inline auto LogLevelToString(ELogLevel level) noexcept -> std::string_view {
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

	template <typename... Args>
	inline void Log(ELogLevel level, std::string_view category, std::format_string<Args...> fmt, Args&&... args) {
		if (m_level > level)
			return;

		std::scoped_lock _(m_mutex);
		m_file << std::format("{}: [@{}] ", LogLevelToString(level), category);
		m_file << std::format(fmt, std::forward<Args>(args)...);
		m_file << "\n";
	}

	inline void SetLevel(ELogLevel level) noexcept {
		m_level = level;
		Log(INFO, "Logger", "Log level is now {}", LogLevelToString(m_level));
	}

	inline auto GetLevel() const noexcept -> ELogLevel { return m_level; }
};

template <size_t N> struct TStringLiteral final {
	char value[N];

	constexpr TStringLiteral(const char (&str)[N]) { std::copy_n(str, N, value); }
};

export template <TStringLiteral Name> class TModule {
public:
	using enum ELogLevel;
	TModule() { Log(ELogLevel::INFO, "Initializing module"); }
	~TModule() { Log(ELogLevel::INFO, "Uninitializing module"); }

	template <typename... Args> inline void Log(ELogLevel level, std::format_string<Args...> fmt, Args&&... args) {
		Logger::GetInstance().Log(level, Name.value, fmt, std::forward<Args>(args)...);
	}

	inline void LogException(const std::exception& exception) {
		Log(ERROR, "C++ exception. \n What: '{}'", exception.what());
	}
};
