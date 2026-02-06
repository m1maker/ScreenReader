// Event listener interface
#pragma once
#include <Core/Event.h>
#include <memory>
#include <vector>
#include <Core/Device.h>

/*
A event listener is a platform-specific interface that processes events and converts them to a common screen reader's IEvent.
*/

class IEventListener {
public:

	IEventListener() = default;
	virtual ~IEventListener() = default;

	virtual void ListenDevice(const EDeviceType& device, bool listen = true) = 0;
};

