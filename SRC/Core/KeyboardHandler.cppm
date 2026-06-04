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
#include <array>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <utility>
export module Core.KeyboardHandler;
import Core.Action;
import Core.Event;
import Core.Logger;
import Core.KeyInfo;
import Core.Singleton;
import Core.Timer;

using KeycodeArray = std::array<std::atomic<unsigned char>, KEYCODE_COUNT>;

export class KeyboardHandler final : TModule<"KeyboardHandler">, public TSingleton<KeyboardHandler> {
	struct SActionInfo final {
		uint32_t id{0};
		ActionCallback<SHotkeyInfo> executable{nullptr};
		bool hook{false};
	};

	std::unordered_map<SHotkeyInfo, SActionInfo> m_actions;
	mutable std::mutex m_actionsMutex;

	KeycodeArray m_keysDown;

	/*
	These are the modifier keys that screen reader uses for its ke bindings.
	As a result of handling, one of these modifiers turns into a generic CKeyboardEvent::MODIFIER_SCREEN_READER.

	Let's set a timer that, when user quickly press one of these modifiers, should let it pass to the OS.
	*/
	ModifierMask m_hookedModifiers{EModifier::MODIFIER_INSERT + EModifier::MODIFIER_CAPS_LOCK};
	mutable CTimer m_hookedModifiersTimer;
	static constexpr uint64_t cHookedModifierPressTimeMs = 300;

public:
	[[nodiscard]] auto RegisterAction(SHotkeyInfo hotkey, uint32_t type, bool hook = false) -> bool;
	void UnregisterAction(SHotkeyInfo action);

	[[nodiscard]] auto IsHooked(EKeycode keycode) const -> bool;

	void Handle(CKeyboardEvent& event);

	[[nodiscard]] auto GetKeysDown() const -> KeycodeMask;
	[[nodiscard]] auto GetModifiers() const -> ModifierMask;

	void ResetState();
};
