#pragma once
#include "Action.h"
#include "Event.h"
#include <unordered_map>
#include <functional>
#include <mutex>
#include "Singleton.h"

class CKeyboardHandler final : public IActionHandler<EDeviceType::KEYBOARD, CKeyboardEvent::SHotkeyInfo> {
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
	unsigned char m_modifiers;

	unsigned char m_hookedModifiers{CKeyboardEvent::MODIFIER_INSERT};

	explicit CKeyboardHandler() = default;
	~CKeyboardHandler() override = default;
public:

	[[nodiscard]] auto RegisterAction(const CKeyboardEvent::SHotkeyInfo& hotkey, uint32_t type, bool hook = false) -> bool override;
	void UnregisterAction(const CKeyboardEvent::SHotkeyInfo& action) override;

	[[nodiscard]] auto IsHooked(const CKeyboardEvent::SHotkeyInfo& hotkey) const -> bool override;

	void Handle(CKeyboardEvent& event);

	[[nodiscard]] auto IsKeyDown(const CKeyboardEvent::EKeycode& keycode) const -> bool;
	[[nodiscard]] auto GetModifiers() const -> unsigned char { return m_modifiers; }

	void ResetState();
};

#define g_keyboardHandler CSingleton<CKeyboardHandler>::GetInstance()
