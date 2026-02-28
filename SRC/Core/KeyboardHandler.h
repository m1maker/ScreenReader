#pragma once
#include "Event.h"
#include "Singleton.h"

#include <Traits/ActionHandler.h>
#include <functional>
#include <mutex>
#include <unordered_map>
import Core.Action;
import Core.EventHandler;
import Core.Timer;

class CKeyboardHandler final {
	DeclareSingleton(CKeyboardHandler);
	struct SActionInfo final {
		uint32_t id{0};
		ActionCallback<CKeyboardEvent::SHotkeyInfo> executable{nullptr};
		bool hook{false};
	};

	friend class CEventHandler;

	std::unordered_map<CKeyboardEvent::SHotkeyInfo, SActionInfo> m_actions;

	mutable std::mutex m_mutex;
	std::unordered_map<CKeyboardEvent::EKeycode, bool> m_keysDown;
	unsigned char m_modifiers{};
	/*
	These are the modifier keys that screen reader uses for its ke bindings.
	As a result of handling, one of these modifiers turns into a generic CKeyboardEvent::MODIFIER_SCREEN_READER.

	Let's set a timer that, when user quickly press one of these modifiers, should let it pass to the OS.
	*/
	unsigned char m_hookedModifiers{CKeyboardEvent::MODIFIER_INSERT | CKeyboardEvent::MODIFIER_CAPS_LOCK};
	mutable CTimer m_hookedModifiersTimer;
	static inline constexpr const uint64_t cHookedModifierPressTimeMs = 300;

	explicit CKeyboardHandler() = default;
	~CKeyboardHandler() = default;

public:
	[[nodiscard]] auto RegisterAction(const CKeyboardEvent::SHotkeyInfo& hotkey, uint32_t type, bool hook = false)
		-> bool;
	void UnregisterAction(const CKeyboardEvent::SHotkeyInfo& action);

	[[nodiscard]] auto IsHooked(const CKeyboardEvent::SHotkeyInfo& hotkey) const -> bool;

	void Handle(CKeyboardEvent& event);

	[[nodiscard]] auto IsKeyDown(const CKeyboardEvent::EKeycode& keycode) const -> bool;
	[[nodiscard]] auto GetModifiers() const -> unsigned char { return m_modifiers; }

	void ResetState();
};

#define g_keyboardHandler CSingleton<CKeyboardHandler>::GetInstance()
