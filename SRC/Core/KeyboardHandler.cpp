module;
#include <mutex>
#include <utility>
module Core.KeyboardHandler;
import Core.KeyMeta;

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

[[nodiscard]] auto KeyboardHandler::GetModifiers() const -> ModifierMask {
	ModifierMask mask;

	for (size_t i = KEYCODE_NONE; i < KEYCODE_COUNT; ++i) {
		auto keycode = static_cast<EKeycode>(i);
		if (!IsKeycodeInGroup(keycode, EKeyGroup::MODIFIER))
			continue;
		else if (m_keysDown[keycode].load() > 0) {
			auto modifier = GetModifierFromKeycode(static_cast<EKeycode>(i));
			if (modifier != MODIFIER_NONE)
				mask[std::to_underlying(modifier)] = true;
		}
	}

	return mask;
}

void KeyboardHandler::Handle(CKeyboardEvent& event) {
	auto type = event.type;
	auto keycode = event.keycode;
	switch (type) {
	case CKeyboardEvent::KEY_PRESSED:
		m_keysDown[keycode].store(1);
		if (IsKeycodeInGroup(keycode, EKeyGroup::MODIFIER)) {
			// Stop now because actions can only be registered with keycodes.
			return;
		}
		{
			auto modifiers = GetModifiers();
			for (unsigned char i = MODIFIER_NONE; i < MODIFIER_COUNT; ++i) {
				if (modifiers[i] && m_hookedModifiers[i]) {
					modifiers[i] = false;
				}
			}
			SHotkeyInfo hotkey(keycode, modifiers);
			std::scoped_lock _(m_actionsMutex);
			auto it = m_actions.find(hotkey);
			if (it == m_actions.end()) {
				it = m_actions.find(SHotkeyInfo::GetAny());
				if (it == m_actions.end())
					return;
			}

			if (it->second.executable) {
				it->second.executable(hotkey);
			}
		}
		break;
	case CKeyboardEvent::KEY_RELEASED:
		m_keysDown[keycode].store(0);
		break;
	default:
		break;
	}
}

[[nodiscard]] auto KeyboardHandler::GetKeysDown() const -> KeycodeMask {
	KeycodeMask mask;

	for (size_t i = KEYCODE_NONE; i < KEYCODE_COUNT; ++i) {
		mask[i] = m_keysDown[i].load();
	}

	return mask;
}

void KeyboardHandler::ResetState() {
	for (auto&& keycode : m_keysDown) {
		keycode.store(0);
	}
}

[[nodiscard]] auto KeyboardHandler::IsHooked(SHotkeyInfo hotkey) const -> bool {
	return false;
}