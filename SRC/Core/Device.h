#pragma once
#include <string_view>

enum class EDeviceType : unsigned char { NO = 0, KEYBOARD, MOUSE, TOUCHSCREEN };

[[nodiscard]] constexpr auto GetDeviceName(EDeviceType d) noexcept -> std::string_view {
	switch (d) {
	case EDeviceType::KEYBOARD:
		return "Keyboard";
	case EDeviceType::MOUSE:
		return "Mouse";
	case EDeviceType::TOUCHSCREEN:
		return "Touchscreen";
	default:
		return "Unknown";
	}
}
