#include "KeyboardHandler.h"

auto CKeyboardHandler::RegisterAction(const CKeyboardEvent::SHotkeyInfo& hotkey, uint32_t type, bool hook) -> bool {
	if (m_actions.find(hotkey) != m_actions.end()) {
		return false;
	}

	m_actions[hotkey].id = type;
	m_actions[hotkey].executable = SActions<CKeyboardEvent::SHotkeyInfo>::GetStaticExecutable(type);
	m_actions[hotkey].hook = hook;
	return true;
}

void CKeyboardHandler::UnregisterAction(const CKeyboardEvent::SHotkeyInfo& action) {
	m_actions.erase(action);
}

void CKeyboardHandler::Handle(CKeyboardEvent& event) {
	auto hotkey = event.hotkey;
	if (hotkey.modifiers & m_hookedModifiers) {
		hotkey.modifiers &= ~m_hookedModifiers;
		hotkey.modifiers |= CKeyboardEvent::MODIFIER_SCREEN_READER;
	}

	auto it = m_actions.find(hotkey);
	if (it == m_actions.end()) {
		it = m_actions.find(CKeyboardEvent::SHotkeyInfo::GetAny());
		if (it == m_actions.end()) return;
	}

	if (it->second.executable) {
		it->second.executable(event.hotkey);
	}
}

[[nodiscard]] auto CKeyboardHandler::IsKeyDown(const CKeyboardEvent::EKeycode& keycode) const -> bool {
	std::lock_guard<std::mutex> lock(m_mutex);
	auto it = m_keysDown.find(keycode);
	if (it!= m_keysDown.end()) return it->second;
	return false;
}

void CKeyboardHandler::ResetState() {
	std::lock_guard<std::mutex> lock(m_mutex);
	m_keysDown.clear();
	m_modifiers = 0;
}

[[nodiscard]] auto CKeyboardHandler::IsHooked(const CKeyboardEvent::SHotkeyInfo& hotkey) const -> bool {
	if (hotkey.modifiers & m_hookedModifiers) {
		if (m_hookedModifiersTimer.Elapsed() > cHookedModifierPressTimeMs) {
			m_hookedModifiersTimer.Restart();
			return true;
		}

		return false;
	}

	auto it = m_actions.find(hotkey);
	if (it == m_actions.end()) {
		return false;
	}

	return it->second.hook;
}