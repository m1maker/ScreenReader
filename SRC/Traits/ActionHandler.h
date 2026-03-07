#pragma once
import Core.Device;
import Core.Event;

template <typename T> struct DeviceEventTrait final {
	static constexpr EDeviceType value = EDeviceType::NO;
};

template <> struct DeviceEventTrait<CKeyboardEvent::SHotkeyInfo> final {
	static constexpr EDeviceType value = EDeviceType::KEYBOARD;
};

template <typename T> struct ActionHandlerTrait final {};
