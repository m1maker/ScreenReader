#pragma once
#include "Action.h"
#include "Event.h"
#include <unordered_map>
#include <functional>

class CKeyboardHandler final : public IActionHandler<EDeviceType::KEYBOARD, CKeyboardEvent> {
public:

	explicit CKeyboardHandler();
	~CKeyboardHandler();

	[[nodiscard]] bool RegisterAction(const CKeyboardEvent& event, const unsigned int& action_type, ActionInterface& action) override;
	void UnregisterAction(const unsigned int& action) override;
};

