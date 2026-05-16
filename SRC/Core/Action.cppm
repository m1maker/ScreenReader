module;
#include <concepts>
#include <cstdint>
#include <string_view>
export module Core.Action;
import Core.Device;
import Core.EventHandler;
import Core.ObjectNavigator;
import Core.Rotor;
import Core.SpeechSystem;

export enum class EAction : uint32_t {
	NONE = 0,
	STOP_SPEECH,
	STOP_KEYBOARD_HOOKS,
	SPIN_ROTOR_LEFT,
	SPIN_ROTOR_RIGHT,
	ADJUST_ROTOR_UP,
	ADJUST_ROTOR_DOWN,
	ADJUST_ROTOR_ACTIVATE,
	MOVE_OBJECT_NAVIGATOR_UP,
	MOVE_OBJECT_NAVIGATOR_DOWN,
	MOVE_OBJECT_NAVIGATOR_LEFT,
	MOVE_OBJECT_NAVIGATOR_RIGHT,
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

	static auto AdjustRotorUp(const Event&) -> EActionHandleResult {
		Rotor::GetInstance().Adjust<ERotorAdjustmentDirection::UP>();
		return EActionHandleResult::HANDLED;
	}
	static auto AdjustRotorDown(const Event&) -> EActionHandleResult {
		Rotor::GetInstance().Adjust<ERotorAdjustmentDirection::DOWN>();
		return EActionHandleResult::HANDLED;
	}
	static auto AdjustRotorActivate(const Event&) -> EActionHandleResult {
		Rotor::GetInstance().Adjust<ERotorAdjustmentDirection::ACTIVATE>();
		return EActionHandleResult::HANDLED;
	}

	static auto MoveObjectNavigatorUp(const Event&) -> EActionHandleResult {
		ObjectNavigator::GetInstance().Step<EObjectNavigatorStepDirection::UP>();
		return EActionHandleResult::HANDLED;
	}
	static auto MoveObjectNavigatorDown(const Event&) -> EActionHandleResult {
		ObjectNavigator::GetInstance().Step<EObjectNavigatorStepDirection::DOWN>();
		return EActionHandleResult::HANDLED;
	}

	static auto MoveObjectNavigatorLeft(const Event&) -> EActionHandleResult {
		ObjectNavigator::GetInstance().Step<EObjectNavigatorStepDirection::LEFT>();
		return EActionHandleResult::HANDLED;
	}
	static auto MoveObjectNavigatorRight(const Event&) -> EActionHandleResult {
		ObjectNavigator::GetInstance().Step<EObjectNavigatorStepDirection::RIGHT>();
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
		case EAction::ADJUST_ROTOR_UP:
			return &AdjustRotorUp;
		case EAction::ADJUST_ROTOR_DOWN:
			return &AdjustRotorDown;
		case EAction::ADJUST_ROTOR_ACTIVATE:
			return &AdjustRotorActivate;

		case EAction::MOVE_OBJECT_NAVIGATOR_UP:
			return &MoveObjectNavigatorUp;
		case EAction::MOVE_OBJECT_NAVIGATOR_DOWN:
			return &MoveObjectNavigatorDown;

		case EAction::MOVE_OBJECT_NAVIGATOR_LEFT:
			return &MoveObjectNavigatorLeft;
		case EAction::MOVE_OBJECT_NAVIGATOR_RIGHT:
			return &MoveObjectNavigatorRight;

		case EAction::STOP_KEYBOARD_HOOKS:
			return &StopKeyboardHooks;
		default:
			return nullptr;
		}
	}
};
