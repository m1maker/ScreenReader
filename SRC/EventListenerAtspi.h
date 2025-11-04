#pragma once
#include "EventListener.h"
#include <atspi/atspi.h>
#include <memory>
#include <unordered_map>
#include <string>

inline const std::unordered_map<std::string, IEvent::EEventType> cAtspiEventTypeMap = {
	{"object:state-changed:focused", IEvent::FOCUS_GAINED},
	{"object:state-changed:selected", IEvent::SELECTION_CHANGED}
};

[[nodiscard]] constexpr inline IEvent::EEventType GetEventTypeFromString(gchar* type) {
	if (!type || !*type) [[unlikely]] return IEvent::NONE;

	std::string type_str(type);

	auto it = cAtspiEventTypeMap.find(type_str);
	if (it != cAtspiEventTypeMap.end()) {
		return it->second;
	}

	return IEvent::NONE;
}

class CEventListenerAtspi final : public IEventListener {
	std::unique_ptr<AtspiEventListener> m_listener;
public:

	static void OnEventCallback(AtspiEvent* event, void* user_data);

	explicit CEventListenerAtspi();
	~CEventListenerAtspi() = default;

	void Post(std::shared_ptr<IEvent> event) override;

	[[nodiscard]] EventQueue& RequestQueue() override;
};

