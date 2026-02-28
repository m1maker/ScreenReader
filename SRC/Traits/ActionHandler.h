#pragma once
#include <Core/Event.h>
import Core.Device;

template <typename T> struct DeviceEventTrait final {
	static constexpr EDeviceType value = EDeviceType::NO;
};

template <> struct DeviceEventTrait<CKeyboardEvent::SHotkeyInfo> final {
	static constexpr EDeviceType value = EDeviceType::KEYBOARD;
};

template <typename T> struct ActionHandlerTrait final {};
