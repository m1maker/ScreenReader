#include "KeyboardHandler.h"

auto CKeyboardHandler::RegisterAction(const CKeyboardEvent::SHotkeyInfo& hotkey, const unsigned int& action_type, ActionInterface& action) -> bool {
	if (m_actions.find(hotkey) != m_actions.end()) {
		return false;
	}

	m_actions[hotkey].id = action_type;
	m_actions[hotkey].executable = &action;
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
