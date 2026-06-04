/*
* This file is part of the Screen Reader project.
*
* Copyright (C) 2025-2026  Georgiy Bondarenko (M_maker / m1maker) <georgijbondarenko248@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
			auto modifier = GetModifierFromKeycode(keycode);
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
		{
			auto modifiers = GetModifiers();
			for (unsigned char i = MODIFIER_NONE; i < MODIFIER_COUNT; ++i) {
				if (modifiers[i] && m_hookedModifiers[i]) {
					modifiers[i] = false;
					modifiers[std::to_underlying(MODIFIER_SCREEN_READER)] = true;
					break;
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

[[nodiscard]] auto KeyboardHandler::IsHooked(EKeycode keycode) const -> bool {
	auto modifier = GetModifierFromKeycode(keycode);
	if (modifier != MODIFIER_NONE && m_hookedModifiers[std::to_underlying(modifier)]) {
		return true;
	}

	auto modifiers = GetModifiers();
	for (unsigned char i = MODIFIER_NONE; i < MODIFIER_COUNT; ++i) {
		if (modifiers[i] && m_hookedModifiers[i]) {
			return true;
		}
	}

	return false;
}
