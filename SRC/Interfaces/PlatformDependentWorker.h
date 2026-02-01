// Platform dependent worker interface.
#pragma once
#include <Core/Logger.h>
#include <string_view>

/*
A platform dependent worker is the screen reader's most basic and main loop.
*/

class IPlatformDependentWorker {
public:

	enum EPlatformError : unsigned char {
		SUCCESS = 0,
		IPC_FAILURE,
		SERVICE_NOT_FOUND,
		ACCESS_DENIED,
		RESOURCE_EXHAUSTED,
		NOT_IMPLEMENTED,
		TIMED_OUT,
		FAIL
	};

	[[nodiscard]] static constexpr auto PlatformErrorToString(const EPlatformError& error) -> std::string_view {
		switch (error) {
			case SUCCESS:
				return "Success: The platform operation completed without errors.";
			case IPC_FAILURE:
				return "IPC Failure: Communication between the screen reader and the OS subsystem has failed.";
			case SERVICE_NOT_FOUND:
				return "Service Not Found: A critical system service is not running or could not be located in the current session.";
			case ACCESS_DENIED:
				return "Access Denied: The operating system refused the request.";
			case RESOURCE_EXHAUSTED:
				return "Resource Exhausted: The system has run out of necessary resources to complete the task.";
			case NOT_IMPLEMENTED:
				return "Not Implemented: The requested feature is not supported by the current operating system version or the specific platform backend.";
			case TIMED_OUT:
				return "Operation Timed Out: The platform backend failed to respond within the allowed timeframe.";
			case FAIL:
			default:
				return "Unknown Platform Error: An unexpected low-level error occurred that does not map to any predefined category.";
		}
	}

protected:
	void PushError(const EPlatformError& error) const noexcept {
		g_logger.Log(CLogger::ERROR, "Platform", std::string(PlatformErrorToString(error)));
	}
public:

	IPlatformDependentWorker() = default;
	virtual ~IPlatformDependentWorker() = default;

	/*
	The Loop function should run as long as g_running is true.
	*/
	virtual void Loop() = 0;

	virtual void Throw(const void* pResult) noexcept = 0;
};

