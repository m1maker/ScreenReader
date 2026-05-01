// Platform dependent worker implementation for Linux.
module;
#include <atspi/atspi.h>
#include <chrono>
#include <csignal>
#include <thread>
#include <unistd.h>
export module Platforms.Linux.Worker;
import Core.AppState;
import Core.PlatformError;

/*
We will also handle signals here to ensure safe exit.
*/
export class CPlatformDependentWorkerLinux final {
	bool m_atspiInitialized{false};

	enum class EDbusError : signed int {
		FAILED = 0,
		NO_MEMORY,
		SERVICE_UNKNOWN,
		NAME_HAS_NO_OWNER,
		NO_REPLY,
		IO_ERROR,
		BAD_ADDRESS,
		NOT_SUPPORTED,
		LIMITS_EXCEEDED,
		ACCESS_DENIED,
		AUTH_FAILED,
		NO_SERVER,
		TIMEOUT,
		NO_NETWORK,
		ADDRESS_IN_USE,
		DISCONNECTED,
		INVALID_ARGS,
		FILE_NOT_FOUND,
		FILE_EXISTS,
		UNKNOWN_METHOD,
		TIMED_OUT,
		MATCH_RULE_NOT_FOUND,
		MATCH_RULE_INVALID,
		SPAWN_EXEC_FAILED,
		SPAWN_FORK_FAILED,
		SPAWN_CHILD_EXITED,
		SPAWN_CHILD_SIGNALED,
		SPAWN_FAILED,
		SPAWN_SETUP_FAILED,
		SPAWN_CONFIG_INVALID,
		SPAWN_SERVICE_INVALID,
		SPAWN_SERVICE_NOT_FOUND,
		SPAWN_PERMISSIONS_INVALID,
		SPAWN_FILE_INVALID,
		SPAWN_NO_MEMORY,
		UNIX_PROCESS_ID_UNKNOWN,
		INVALID_SIGNATURE,
		INVALID_FILE_CONTENT,
		SELINUX_SECURITY_CONTEXT_UNKNOWN,
		ADT_AUDIT_DATA_UNKNOWN,
		OBJECT_PATH_IN_USE,
		UNKNOWN_OBJECT,
		UNKNOWN_INTERFACE,
		UNKNOWN_PROPERTY,
		PROPERTY_READ_ONLY
	};

	static auto GerrorToPlatformError(const GError* error) noexcept -> EPlatformError {
		using enum EPlatformError;
		if (!error)
			return SUCCESS;

		EPlatformError plat_err{FAIL};

		if (error->domain > 0) {
			auto error_code = static_cast<EDbusError>(error->code);
			switch (error_code) {
			case EDbusError::ACCESS_DENIED:
			case EDbusError::AUTH_FAILED:
				plat_err = ACCESS_DENIED;
				break;
			case EDbusError::SERVICE_UNKNOWN:
			case EDbusError::NAME_HAS_NO_OWNER:
			case EDbusError::NO_SERVER:
			case EDbusError::DISCONNECTED:
				plat_err = SERVICE_NOT_FOUND;
				break;
			case EDbusError::NO_REPLY:
			case EDbusError::TIMEOUT:
			case EDbusError::TIMED_OUT:
				plat_err = TIMED_OUT;
				break;
			case EDbusError::NO_MEMORY:
			case EDbusError::SPAWN_NO_MEMORY:
			case EDbusError::LIMITS_EXCEEDED:
				plat_err = RESOURCE_EXHAUSTED;
				break;
			case EDbusError::NOT_SUPPORTED:
			case EDbusError::UNKNOWN_METHOD:
			case EDbusError::UNKNOWN_INTERFACE:
				plat_err = NOT_IMPLEMENTED;
				break;
			default:
				plat_err = FAIL;
				break;
			}
		}

		return plat_err;
	}

	struct ::sigaction m_signalAction{};

	static void HandleSignal(int signal) {
		switch (signal) {
		case SIGINT:
		case SIGTERM:
			g_running.store(false);
			break;
		default:
			break;
		}
	}

public:
	explicit CPlatformDependentWorkerLinux() {
		memset(&m_signalAction, 0, sizeof(m_signalAction));
		m_signalAction.sa_handler = &HandleSignal;
		sigemptyset(&m_signalAction.sa_mask);
		m_signalAction.sa_flags = 0;

		sigaction(SIGINT, &m_signalAction, nullptr);
		sigaction(SIGTERM, &m_signalAction, nullptr);
		int result = atspi_init();
		m_atspiInitialized = result == 0 || result == 1 ? true : false;
	}

	~CPlatformDependentWorkerLinux() {
		if (m_atspiInitialized)
			atspi_exit();
	}

	void Loop() {
		if (m_atspiInitialized) {
			atspi_event_main();
			auto* context = g_main_context_default();
			while (g_main_context_pending(context)) {
				g_main_context_iteration(context, FALSE);
			}
		}
		else {
			while (g_running.load()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(5));
			}
		}
	}
};
