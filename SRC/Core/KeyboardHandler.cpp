#include "KeyboardHandler.h"

auto CKeyboardHandler::RegisterAction(const CKeyboardEvent::SHotkeyInfo& hotkey, const unsigned int& action_type, ActionInterface& action, bool hook) -> bool {
	if (m_actions.find(hotkey) != m_actions.end()) {
		return false;
	}

	m_actions[hotkey].id = action_type;
	m_actions[hotkey].executable = &action;
	m_actions[hotkey].hook = hook;
	return true;
}

void CKeyboardHandler::UnregisterAction(const CKeyboardEvent::SHotkeyInfo& action) {
	m_actions.erase(action);
}

void CKeyboardHandler::Handle(CKeyboardEvent& event) {
	auto it = m_actions.find(event.hotkey);
	if (it == m_actions.end()) {
		it = m_actions.find(CKeyboardEvent::SHotkeyInfo::GetAny());
		if (it == m_actions.end()) return;
	}

	if (it->second.executable) {
		it->second.executable->Execute(event.hotkey);
	}
}

[[nodiscard]] auto CKeyboardHandler::IsKeyDown(const CKeyboardEvent::EKeycode& keycode) const -> bool {
	auto it = m_keysDown.find(keycode);
	if (it!= m_keysDown.end()) return it->second;
	return false;
}

void CKeyboardHandler::ResetState() {
	m_keysDown.clear();
	m_modifiers = 0;
}

[[nodiscard]] auto CKeyboardHandler::IsHooked(const CKeyboardEvent::SHotkeyInfo& hotkey) const -> bool {
	if (hotkey.modifiers & m_hookedModifiers) return true;
	auto it = m_actions.find(hotkey);
	if (it == m_actions.end()) {
		return false;
	}

	return it->second.hook;
}