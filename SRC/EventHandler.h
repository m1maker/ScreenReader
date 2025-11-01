#pragma once
#include "EventListener.h"
#include "Singleton.h"
#include <memory>

class CEventHandler final {
	std::unique_ptr<IEventListener> m_listener;
public:
	explicit CEventHandler();
	~CEventHandler() = default;

	void Handle();
};

#define g_eventHandler CSingleton<CEventHandler>::GetInstance()

