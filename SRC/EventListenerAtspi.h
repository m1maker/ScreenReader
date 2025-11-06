// AT_SPI's event listener.
#pragma once
#include "EventListener.h"
#include <atspi/atspi.h>
#include <memory>
#include <unordered_map>
#include <string>

/*
AT-SPI has a listener where you need to register the required events one by one.
But there's one very strange thing: all of these types are strings. Look at this code.
*/
inline const std::unordered_map<std::string, IEvent::EEventType> cAtspiEventTypeMap = {
	{"object:state-changed:focused", IEvent::FOCUS_GAINED},
	{"object:state-changed:checked", IEvent::STATE_CHANGED},
	{"object:state-changed:selected", IEvent::SELECTION_CHANGED},
	{"object:property-change:accessible-parent", IEvent::PARENT_UPDATED},
	{"object:property-change:accessible-value", IEvent::VALUE_CHANGED},
	{"object:text-caret-moved", IEvent::CURSOR_MOVED}
};

[[nodiscard]] constexpr inline IEvent::EEventType GetEventTypeFromString(gchar* type) {
	if (!type) [[unlikely]] return IEvent::NONE;

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

