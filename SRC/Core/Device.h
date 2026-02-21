#pragma once
#include <string_view>

enum class EDeviceType : unsigned char { NO = 0, KEYBOARD, MOUSE, TOUCHSCREEN };

[[nodiscard]] constexpr std::string_view GetDeviceName(EDeviceType d) noexcept {
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
