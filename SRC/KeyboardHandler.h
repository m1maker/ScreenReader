#pragma once
#include "Action.h"
#include "Event.h"
#include <unordered_map>
#include <functional>
#include "Singleton.h"

class CKeyboardHandler final : public IActionHandler<EDeviceType::KEYBOARD, CKeyboardEvent::SHotkeyInfo> {
	DeclareSingleton(CKeyboardHandler);
	struct SActionInfo final {
		unsigned int id{0};
		ActionInterface* executable{nullptr};
	};

	friend class CEventHandler;

	std::unordered_map<CKeyboardEvent::SHotkeyInfo, SActionInfo> m_actions;
	std::unordered_map<CKeyboardEvent::EKeycode, bool> m_keysDown;
	unsigned char m_modifiers;

	explicit CKeyboardHandler() = default;
	~CKeyboardHandler() override = default;
public:

	[[nodiscard]] auto RegisterAction(const CKeyboardEvent::SHotkeyInfo& hotkey, const unsigned int& action_type, ActionInterface& action) -> bool override;
	void UnregisterAction(const CKeyboardEvent::SHotkeyInfo& action) override;

	void Handle(CKeyboardEvent& event);

	[[nodiscard]] auto IsKeyDown(const CKeyboardEvent::EKeycode& keycode) const -> bool;
	[[nodiscard]] auto GetModifiers() const -> unsigned char { return m_modifiers; }

	void ResetState();
};

#define g_keyboardHandler CSingleton<CKeyboardHandler>::GetInstance()
