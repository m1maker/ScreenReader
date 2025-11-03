#pragma once
#include "EventListener.h"
#include "Singleton.h"
#include <memory>

class CEventHandler final {
	std::shared_ptr<IEventListener> m_listener;
public:
	explicit CEventHandler();
	~CEventHandler() = default;

	void Handle();

	inline std::shared_ptr<IEventListener> GetListener() { return m_listener; }
};

#define g_eventHandler CSingleton<CEventHandler>::GetInstance()

