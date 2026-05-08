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

enum class EObjectNavigatorError : unsigned char { MOVED = 0, NO_NEXT, NO_PREVIOUS, NO_CHILDREN, NO_PARENT, NO_VALID };

template <typename T = void> using ObjectNavigatorResult = std::expected<T, EObjectNavigatorError>;

export enum class EObjectNavigatorStepDirection : unsigned char { LEFT, RIGHT, UP, DOWN };

export class ObjectNavigator : TModule<"ObjectNavigator">, public TSingleton<ObjectNavigator> {
	CObjectProxy m_objectInFocus;

	void Output() {
		auto& output_manager = OutputManager::GetInstance();
		output_manager.Stop();
		CObjectEvent object_event{.type = EObjectEventType::FOCUS_GAINED, .object = m_objectInFocus};
		output_manager.Output(object_event);
	}

public:
	explicit ObjectNavigator() = default;

	template <EObjectNavigatorStepDirection Direction> void Step() {}

	void SetFocus(CObjectProxy obj) {
		if (!obj.IsValid()) [[unlikely]]
			return;

		m_objectInFocus = obj;
	}
};
