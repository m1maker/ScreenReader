module;
#include <concepts>
#include <cstdint>
#include <string_view>
export module Core.Action;
import Core.Device;
import Core.EventHandler;
import Core.Rotor;
import Core.SpeechSystem;

export enum class EAction : uint32_t {
	NONE = 0,
	STOP_SPEECH,
	STOP_KEYBOARD_HOOKS,
	SPIN_ROTOR_LEFT,
	SPIN_ROTOR_RIGHT,
	USER
};

export enum class EActionHandleResult : unsigned char {
	NOT_HANDLED = 0,
	HANDLED,
};

export template <typename EventType> using ActionCallback = EActionHandleResult (*)(const EventType&);

export template <typename Event> struct TActions final {
	static auto StopSpeech(const Event&) -> EActionHandleResult {
		SpeechSystem::GetInstance().Interrupt();
		return EActionHandleResult::HANDLED;
	}

	static auto StopKeyboardHooks(const Event&) -> EActionHandleResult {
		auto listener = EventHandler::GetInstance().GetListener();
		if (listener) [[likely]] {
			listener->ListenDevice(EDeviceType::KEYBOARD, false);
			SpeechSystem::GetInstance().Speak("Stop listening keyboard");
			return EActionHandleResult::HANDLED;
		}

		return EActionHandleResult::NOT_HANDLED;
	}

	static auto SpinRotorLeft(const Event&) -> EActionHandleResult {
		Rotor::GetInstance().Spin<ERotorSpinDirection::LEFT>();
		return EActionHandleResult::HANDLED;
	}
	static auto SpinRotorRight(const Event&) -> EActionHandleResult {
		Rotor::GetInstance().Spin<ERotorSpinDirection::RIGHT>();
		return EActionHandleResult::HANDLED;
	}

	[[nodiscard]] static auto GetStaticExecutable(uint32_t type) -> ActionCallback<Event> {
		switch (static_cast<EAction>(type)) {
		case EAction::STOP_SPEECH:
			return &StopSpeech;
		case EAction::SPIN_ROTOR_LEFT:
			return &SpinRotorLeft;
		case EAction::SPIN_ROTOR_RIGHT:
			return &SpinRotorRight;

		case EAction::STOP_KEYBOARD_HOOKS:
			return &StopKeyboardHooks;
		default:
			return nullptr;
		}
	}
};
