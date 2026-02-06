// Event listener interface
#pragma once
#include <Core/Event.h>
#include <memory>
#include <vector>
#include <Core/Device.h>

/*
A event listener is a platform-specific interface that processes events and converts them to a common screen reader's IEvent.
*/

using EventQueue = std::vector<CEvent>;

class IEventListener {
protected:
	EventQueue m_eventQueue;
public:

	IEventListener() = default;
	virtual ~IEventListener() = default;

	virtual void Post(const CEvent& event) = 0;

	[[nodiscard]] virtual auto RequestQueue() -> EventQueue& = 0;

	virtual void ListenDevice(const EDeviceType& device, bool listen = true) = 0;
};

