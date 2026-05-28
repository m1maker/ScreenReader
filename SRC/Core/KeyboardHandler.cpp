module;
#include <mutex>
module Core.KeyboardHandler;

auto KeyboardHandler::RegisterAction(SHotkeyInfo hotkey, uint32_t type, bool hook) -> bool {
	std::scoped_lock _(m_actionsMutex);
	if (m_actions.find(hotkey) != m_actions.end()) {
		return false;
	}

	m_actions[hotkey].id = type;
	m_actions[hotkey].executable = TActions<SHotkeyInfo>::GetStaticExecutable(type);
	m_actions[hotkey].hook = hook;
	return true;
}

void KeyboardHandler::UnregisterAction(SHotkeyInfo action) {
	std::scoped_lock _(m_actionsMutex);
	m_actions.erase(action);
}

void KeyboardHandler::Handle(CKeyboardEvent& event) {
	auto type = event.type;
	auto hotkey = event.hotkey;
	switch (type) {
	case CKeyboardEvent::KEY_PRESSED:
		m_keysDown[hotkey.keycode].store(1);
		m_modifiers.store(hotkey.modifiers);
		{
			if (hotkey.modifiers & m_hookedModifiers) {
				hotkey.modifiers &= ~m_hookedModifiers;
				hotkey.modifiers |= MODIFIER_SCREEN_READER;
			}

			std::scoped_lock _(m_actionsMutex);
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
		break;
	case CKeyboardEvent::KEY_RELEASED:
		m_keysDown[hotkey.keycode].store(false);
		m_modifiers.store(hotkey.modifiers);
		break;
	default:
		break;
	}
}

[[nodiscard]] auto KeyboardHandler::GetKeysDown() const -> KeysDownArray {
	KeysDownArray array;

	for (size_t i = KEYCODE_NONE; i < KEYCODE_COUNT; ++i) {
		array[i] = m_keysDown[i].load();
	}

	return array;
}

void KeyboardHandler::ResetState() {
	for (auto&& keycode : m_keysDown) {
		keycode.store(0);
	}
	m_modifiers.store(0);
}

[[nodiscard]] auto KeyboardHandler::IsHooked(SHotkeyInfo hotkey) const -> bool {
	if (hotkey.modifiers & m_hookedModifiers) {
		auto modifier = GetModifierFromKeycode(hotkey.keycode);
		if (modifier == MODIFIER_NONE)
			return true;
		else if (modifier & m_hookedModifiers && m_hookedModifiersTimer.Elapsed() < cHookedModifierPressTimeMs) {
			m_hookedModifiersTimer.Restart();
			return false;
		}
	}

	return false;
}