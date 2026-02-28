// Event listener trait
#pragma once
#include <Core/StaticInterface.h>
import Core.Device;

/*
A event listener is a platform-specific trait that processes events and converts them to a common screen reader's
IEvent.
*/

using ThreadFunction = void (*)(void*);
template <typename Derived> class TEventListener {
public:
	BindStaticInterface(Derived);

	void ListenDevice(EDeviceType type, bool listen = true) { return Impl().do_ListenDevice(type, listen); }

	void PushToMainThread(ThreadFunction function, void* pUserData) {
		return Impl().do_PushToMainThread(function, pUserData);
	}
};
