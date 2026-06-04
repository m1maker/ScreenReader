/*
* This file is part of the Screen Reader project.
*
* Copyright (C) 2025-2026  Georgiy Bondarenko (M_maker / m1maker) <georgijbondarenko248@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

module;
#include <string_view>
export module Core.PlatformError;

export enum class EPlatformError : unsigned char {
	SUCCESS = 0,
	IPC_FAILURE,
	SERVICE_NOT_FOUND,
	ACCESS_DENIED,
	RESOURCE_EXHAUSTED,
	NOT_IMPLEMENTED,
	TIMED_OUT,
	FAIL
};

export [[nodiscard]] constexpr auto PlatformErrorToString(EPlatformError error) -> std::string_view {
	using enum EPlatformError;
	switch (error) {
	case SUCCESS:
		return "Success: The platform operation completed without errors.";
	case IPC_FAILURE:
		return "IPC Failure: Communication between the screen reader and the OS subsystem has failed.";
	case SERVICE_NOT_FOUND:
		return "Service Not Found: A critical system service is not running or could not be located in the current "
			   "session.";
	case ACCESS_DENIED:
		return "Access Denied: The operating system refused the request.";
	case RESOURCE_EXHAUSTED:
		return "Resource Exhausted: The system has run out of necessary resources to complete the task.";
	case NOT_IMPLEMENTED:
		return "Not Implemented: The requested feature is not supported by the current operating system version or "
			   "the specific platform backend.";
	case TIMED_OUT:
		return "Operation Timed Out: The platform backend failed to respond within the allowed timeframe.";
	case FAIL:
	default:
		return "Unknown Platform Error: An unexpected low-level error occurred that does not map to any predefined "
			   "category.";
	}
}
