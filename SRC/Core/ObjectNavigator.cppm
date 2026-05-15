module;
#include <expected>
#include <string_view>
export module Core.ObjectNavigator;
import Core.Event;
import Core.Logger;
import Core.Object;
import Core.OutputManager;
import Core.Singleton;
import Proxies.Object;

enum class EObjectNavigatorStatus : unsigned char { MOVED = 0, NO_NEXT, NO_PREVIOUS, NO_CHILDREN, NO_PARENT, NO_VALID };

export enum class EObjectNavigatorStepDirection : unsigned char { LEFT, RIGHT, UP, DOWN };

export class ObjectNavigator : TModule<"ObjectNavigator">, public TSingleton<ObjectNavigator> {
	CObjectProxy m_objectInFocus;

	EObjectNavigatorStatus m_lastStatus{EObjectNavigatorStatus::NO_VALID};

	void Output() {
		if (m_lastStatus != EObjectNavigatorStatus::MOVED) {
			// Announce status
			m_lastStatus = EObjectNavigatorStatus::NO_VALID;
		}

		auto& output_manager = OutputManager::GetInstance();
		output_manager.Stop();
		CObjectEvent object_event{.type = EObjectEventType::FOCUS_GAINED, .object = m_objectInFocus};
		output_manager.Output(object_event);
	}

public:
	explicit ObjectNavigator() = default;

	template <EObjectNavigatorStepDirection Direction> void Step() {
		if constexpr (Direction == EObjectNavigatorStepDirection::UP) {
			auto parent = m_objectInFocus.GetParent();
			if (parent) {
				m_objectInFocus = *parent;
				m_lastStatus = EObjectNavigatorStatus::MOVED;
			}
			else
				m_lastStatus = EObjectNavigatorStatus::NO_PARENT;
		}
		else if constexpr (Direction == EObjectNavigatorStepDirection::DOWN) {
			auto children = m_objectInFocus.GetChildAt(0);
			if (children) {
				m_objectInFocus = *children;
				m_lastStatus = EObjectNavigatorStatus::MOVED;
			}
			else
				m_lastStatus = EObjectNavigatorStatus::NO_CHILDREN;
		}

		Output();
	}

	void SetFocus(CObjectProxy obj) {
		if (!obj.IsValid()) [[unlikely]]
			return;

		m_objectInFocus = obj;
	}
};
