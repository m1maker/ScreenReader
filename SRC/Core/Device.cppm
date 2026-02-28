module;
#include <string_view>
export module Core.Device;

export enum class EDeviceType : unsigned char { NO = 0, KEYBOARD, MOUSE, TOUCHSCREEN };

export [[nodiscard]] constexpr auto GetDeviceName(EDeviceType d) noexcept -> std::string_view {
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
