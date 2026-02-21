#pragma once

#include "App.h"
#include "Device.h"
#include "Logger.h"
#include "SpeechEngine.h"

#include <concepts>
#include <string_view>

enum class EAction : uint32_t { NONE = 0, STOP_SPEECH, STOP_KEYBOARD_HOOKS, USER };

enum class EActionHandleResult : unsigned char {
	NOT_HANDLED = 0,
	HANDLED,
};

template <typename EventType> using ActionCallback = EActionHandleResult (*)(const EventType&);

template <EDeviceType D, typename T> class IActionHandler {
public:
	virtual ~IActionHandler() = default;

	auto LogRegistration(this auto&& self) {
		g_logger.Log(
			CLogger::INFO, "Action handler", std::string("Registered handler for ") + std::string(GetDeviceName(D)));
	}

	virtual auto RegisterAction(const T& event, uint32_t type, bool hook = false) -> bool = 0;
	virtual void UnregisterAction(const T& event) = 0;

	[[nodiscard]] virtual auto IsHooked(const T& event) const -> bool = 0;
};

template <typename T> struct SActions final {
	static auto StopSpeech(const T&) -> EActionHandleResult {
		g_speechEngine.Stop();
		return EActionHandleResult::HANDLED;
	}

	static auto StopKeyboardHooks(const T&) -> EActionHandleResult {
		if (auto listener = g_eventHandler.GetListener(); listener) [[likely]] {
			listener->ListenDevice(EDeviceType::KEYBOARD, false);
			g_speechEngine.Speak("Stop listening keyboard");
			return EActionHandleResult::HANDLED;
		}
		return EActionHandleResult::NOT_HANDLED;
	}

	[[nodiscard]] static auto GetStaticExecutable(uint32_t type) -> ActionCallback<T> {
		switch (static_cast<EAction>(type)) {
		case EAction::STOP_SPEECH:
			return &StopSpeech;
		case EAction::STOP_KEYBOARD_HOOKS:
			return &StopKeyboardHooks;
		default:
			return nullptr;
		}
	}
};
