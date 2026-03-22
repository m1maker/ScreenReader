module;
#include <mutex>
module Core.KeyboardHandler;

auto KeyboardHandler::RegisterAction(SHotkeyInfo hotkey, uint32_t type, bool hook) -> bool {
	if (m_actions.find(hotkey) != m_actions.end()) {
		return false;
	}

	m_actions[hotkey].id = type;
	m_actions[hotkey].executable = TActions<SHotkeyInfo>::GetStaticExecutable(type);
	m_actions[hotkey].hook = hook;
	return true;
}

void KeyboardHandler::UnregisterAction(SHotkeyInfo action) {
	m_actions.erase(action);
}

void KeyboardHandler::Handle(CKeyboardEvent& event) {
	auto hotkey = event.hotkey;
	if (hotkey.modifiers & m_hookedModifiers) {
		hotkey.modifiers &= ~m_hookedModifiers;
		hotkey.modifiers |= MODIFIER_SCREEN_READER;
	}

	auto it = m_actions.find(hotkey);
	if (it == m_actions.end()) {
		it = m_actions.find(SHotkeyInfo::GetAny());
		if (it == m_actions.end())
			return;
	}

	if (it->second.executable) {
		it->second.executable(event.hotkey);
	}
}

[[nodiscard]] auto KeyboardHandler::IsKeyDown(EKeycode keycode) const -> bool {
	std::scoped_lock lock(m_mutex);
	auto it = m_keysDown.find(keycode);
	if (it != m_keysDown.end())
		return it->second;
	return false;
}

void KeyboardHandler::ResetState() {
	std::scoped_lock lock(m_mutex);
	m_keysDown.clear();
	m_modifiers = 0;
}

[[nodiscard]] auto KeyboardHandler::IsHooked(SHotkeyInfo hotkey) const -> bool {
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