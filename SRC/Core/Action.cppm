module;
#include "Device.h"
#include "EventHandler.h"
#include "Logger.h"
#include "SpeechEngine.h"

#include <concepts>
#include <string_view>
export module Core.Action;

export enum class EAction : uint32_t { NONE = 0, STOP_SPEECH, STOP_KEYBOARD_HOOKS, USER };

export enum class EActionHandleResult : unsigned char {
	NOT_HANDLED = 0,
	HANDLED,
};

export template <typename EventType> using ActionCallback = EActionHandleResult (*)(const EventType&);

export template <typename T> struct SActions final {
	static auto StopSpeech(const T&) -> EActionHandleResult {
		g_speechEngine.Stop();
		return EActionHandleResult::HANDLED;
	}

	static auto StopKeyboardHooks(const T&) -> EActionHandleResult {
		auto listener = g_eventHandler.GetListener();
		if (listener) [[likely]] {
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
