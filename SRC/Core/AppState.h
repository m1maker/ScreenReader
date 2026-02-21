#pragma once
#include <atomic>
#include <string_view>

/*
General return codes from the main function.
*/
class CScreenReaderAppReturnCode final {
public:
	enum EReturnCode : int {
		SUCCESS = 0,
		SUCCESS_ALREADY_INITIALIZED = 2,

		ERROR_UNKNOWN = 100,
		ERROR_LOGGER = 101,
		ERROR_INVALID_PARAMETER = 103,
		ERROR_OUT_OF_MEMORY = 104,
		ERROR_NOT_SUPPORTED = 105,
		ERROR_TIMEOUT = 106,
		ERROR_ACCESS_DENIED = 107,

		ERROR_PLATFORM_WINDOWS = 200,
		ERROR_PLATFORM_LINUX = 201,
		ERROR_PLATFORM_ANDROID = 202,

		ERROR_ACCESSIBILITY_API_UNAVAILABLE = 300,

		ERROR_CONFIGURATION_INVALID = 700,
		ERROR_SETTINGS_LOAD_FAILED = 701,

		ERROR_VERSION_INCOMPATIBLE = 800
	};

private:
	EReturnCode m_returnCode{SUCCESS};

public:
	constexpr CScreenReaderAppReturnCode(const EReturnCode& return_code) : m_returnCode(return_code) {}
	auto operator=(const EReturnCode& return_code) -> CScreenReaderAppReturnCode& {
		m_returnCode = return_code;
		return *this;
	}

	operator EReturnCode() const { return m_returnCode; }
	[[nodiscard]] constexpr auto ToInt() const -> int { return static_cast<int>(m_returnCode); }

	[[nodiscard]] constexpr inline auto ToString() const -> std::string_view {
		switch (m_returnCode) {
		case SUCCESS:
			return "Operation completed successfully";
		case SUCCESS_ALREADY_INITIALIZED:
			return "Operation completed, already initialized";

		case ERROR_UNKNOWN:
			return "An unknown error occurred";
		case ERROR_INVALID_PARAMETER:
			return "Invalid parameter provided";
		case ERROR_OUT_OF_MEMORY:
			return "Insufficient memory available";
		case ERROR_NOT_SUPPORTED:
			return "Operation not supported on this platform";
		case ERROR_TIMEOUT:
			return "Operation timed out";
		case ERROR_ACCESS_DENIED:
			return "Access denied due to insufficient permissions";

		case ERROR_PLATFORM_WINDOWS:
			return "Windows-specific error";
		case ERROR_PLATFORM_LINUX:
			return "Linux-specific error";
		case ERROR_PLATFORM_ANDROID:
			return "Android-specific error";

		case ERROR_ACCESSIBILITY_API_UNAVAILABLE:
			return "Accessibility API not available";

		case ERROR_CONFIGURATION_INVALID:
			return "Invalid configuration detected";
		case ERROR_SETTINGS_LOAD_FAILED:
			return "Failed to load settings";

		case ERROR_VERSION_INCOMPATIBLE:
			return "Version incompatible";

		default:
			return "Unknown return code";
		}
	}
};

/*
If g_running is false, all processes, loops, and threads should exit.
After that, application must return g_retcode.

However, a screen reader is a program that must be robust against exceptions and continue running.
Hopefully, these flags will only be used when we disable the screen reader.
*/
inline std::atomic<bool> g_running{false};
inline constinit CScreenReaderAppReturnCode g_returnCode{CScreenReaderAppReturnCode::SUCCESS};
