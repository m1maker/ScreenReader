#pragma once
#include "App.h"
#include <functional>
#include "Singleton.h"
#include <string_view>
#include <string>
#include "Logger.h"
#include "Device.h"
#include <Core/EventHandler.h>

enum class EAction {
	NONE = 0,
	STOP_SPEECH,
	STOP_KEYBOARD_HOOKS,
	USER
};

enum class EActionHandleResult {
	NOT_HANDLED = 0,
	HANDLED,
};

template<typename ResultType, typename... Args>
class IActionExecutable {
public:

	virtual ~IActionExecutable() = default;
	virtual auto Execute(Args... args) -> ResultType = 0;
};

template<unsigned int A, typename T, typename... Args>
class IAction : public IActionExecutable<T, Args...> {
public:

	virtual ~IAction() = default;

	[[nodiscard]] static constexpr auto GetActionType() -> unsigned int {
		return A;
	}
};

template <EDeviceType D, class T>
class IActionHandler {
public:

	[[nodiscard]] static constexpr auto GetDeviceTypeName() -> std::string_view {
		switch (D) {
			case EDeviceType::KEYBOARD: return "Keyboard";
			case EDeviceType::MOUSE: return "Mouse";
			case EDeviceType::TOUCHSCREEN: return "Touchscreen";
			default: return "Unknown";
		}
	}

	using ActionInterface = IActionExecutable<EActionHandleResult, const T&>;

	IActionHandler() {
		g_logger.Log(CLogger::INFO, "Action handler", "Registered handler for device " + std::string(GetDeviceTypeName()));
	}
	virtual ~IActionHandler() = default;

	[[nodiscard]] static constexpr auto GetDeviceType() -> EDeviceType {
		return D;
	}

	[[nodiscard]] virtual auto RegisterAction(const T& event, const unsigned int& action_type, ActionInterface& action, bool hook = false) -> bool = 0;
	virtual void UnregisterAction(const T& action) = 0;

	[[nodiscard]] virtual auto IsHooked(const T& action) const -> bool = 0;
};

template<typename T>
class CActionStopSpeech final : public IAction<static_cast<unsigned int>(EAction::STOP_SPEECH), EActionHandleResult, const T&> {
public:
	auto Execute(const T& event) -> EActionHandleResult override {
		auto speaker = g_speechEngine;
		speaker.Stop();
		return EActionHandleResult::HANDLED;
	}
};

#define g_actionStopSpeech(T) CSingleton<CActionStopSpeech<T>>::GetInstance()

template<typename T>
class CActionStopKeyboardHooks final : public IAction<static_cast<unsigned int>(EAction::STOP_KEYBOARD_HOOKS), EActionHandleResult, const T&> {
public:
	auto Execute(const T& event) -> EActionHandleResult override {
		auto listener = g_eventHandler.GetListener();
		if (!listener) [[unlikely]] return EActionHandleResult::NOT_HANDLED;
		listener->ListenDevice(EDeviceType::KEYBOARD, false);
		g_speechEngine.Speak("Stop listening keyboard");
		return EActionHandleResult::HANDLED;
	}
};

#define g_actionStopKeyboardHooks(T) CSingleton<CActionStopKeyboardHooks<T>>::GetInstance()
