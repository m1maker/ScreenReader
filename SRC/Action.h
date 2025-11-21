#pragma once
#include "SpeechEngine.h"
#include <functional>

enum class EAction {
	NONE = 0,
	STOP_SPEECH,
	USER
};

enum class EActionHandleResult {
	NOT_HANDLED = 0,
	HANDLED,
};

enum class EDeviceType {
	NO = 0,
	KEYBOARD,
	MOUSE,
	TOUCHSCREEN
};

template<typename ResultType, typename... Args>
class IActionExecutable {
public:

	virtual ~IActionExecutable() = default;
	virtual ResultType Execute(Args... args) = 0;
};

template<unsigned int A, typename T, typename... Args>
class IAction : public IActionExecutable<T, Args...> {
public:

	virtual ~IAction() = default;

	[[nodiscard]] static constexpr unsigned int GetActionType() {
		return A;
	}
};

template <EDeviceType D, class T>
class IActionHandler {
public:

	using ActionInterface = IActionExecutable<EActionHandleResult, const T&>;

	IActionHandler() = default;
	virtual ~IActionHandler() = default;

	[[nodiscard]] static constexpr EDeviceType GetDeviceType() {
		return D;
	}

	[[nodiscard]] virtual bool RegisterAction(const T& event, const unsigned int& action_type, ActionInterface& action) = 0;
	virtual void UnregisterAction(const unsigned int& action) = 0;
};

template<typename T>
class CActionStopSpeech final : public IAction<static_cast<unsigned int>(EAction::STOP_SPEECH), EActionHandleResult, const T&> {
public:
	EActionHandleResult Execute(const T& event) override {
		auto& speaker = g_speechEngine.GetSpeaker();
		if (speaker) speaker->StopSpeech();
		return EActionHandleResult::HANDLED;
	}
};
