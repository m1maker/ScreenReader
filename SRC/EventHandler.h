#pragma once
#include "EventListener.h"
#include <memory>

class CEventHandler final {
	std::unique_ptr<IEventListener> m_listener;
public:
	CEventHandler() = default;
	~CEventHandler() = default;

};

