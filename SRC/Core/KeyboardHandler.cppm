module;
#include <array>
#include <atomic>
#include <mutex>
#include <unordered_map>
export module Core.KeyboardHandler;
import Core.Action;
import Core.Event;
import Core.Logger;
import Core.KeyInfo;
import Core.Singleton;
import Core.Timer;

using KeycodeArray = std::array<std::atomic<unsigned char>, KEYCODE_COUNT>;
using KeyModifierMask = std::atomic<unsigned char>;

export class KeyboardHandler final : TModule<"KeyboardHandler">, public TSingleton<KeyboardHandler> {
	struct SActionInfo final {
		uint32_t id{0};
		ActionCallback<SHotkeyInfo> executable{nullptr};
		bool hook{false};
	};

	std::unordered_map<SHotkeyInfo, SActionInfo> m_actions;
	mutable std::mutex m_actionsMutex;

	KeycodeArray m_keysDown;
	KeyModifierMask m_modifiers{};

	/*
	These are the modifier keys that screen reader uses for its ke bindings.
	As a result of handling, one of these modifiers turns into a generic CKeyboardEvent::MODIFIER_SCREEN_READER.

	Let's set a timer that, when user quickly press one of these modifiers, should let it pass to the OS.
	*/
	KeyModifierMask m_hookedModifiers{MODIFIER_INSERT | MODIFIER_CAPS_LOCK};
	mutable CTimer m_hookedModifiersTimer;
	static constexpr uint64_t cHookedModifierPressTimeMs = 300;

public:
	[[nodiscard]] auto RegisterAction(SHotkeyInfo hotkey, uint32_t type, bool hook = false) -> bool;
	void UnregisterAction(SHotkeyInfo action);

	[[nodiscard]] auto IsHooked(SHotkeyInfo hotkey) const -> bool;

	void Handle(CKeyboardEvent& event);

	[[nodiscard]] auto IsKeyDown(EKeycode keycode) const -> bool;
	[[nodiscard]] auto GetModifiers() const -> unsigned char { return m_modifiers; }

	void ResetState();
};
