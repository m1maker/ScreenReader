// Event listener trait
module;
#include <Core/StaticInterface.h>
export module Traits.EventListener;
import Core.Device;

/*
A event listener is a platform-specific trait that processes events and converts them to a common screen reader's
IEvent.
*/

export using ThreadFunction = void (*)(void*);
export template <typename Derived> class TEventListener {
public:
	BindStaticInterface(Derived);

	void ListenDevice(EDeviceType type, bool listen = true) { return Impl().do_ListenDevice(type, listen); }

	void PushToMainThread(ThreadFunction function, void* pUserData) {
		return Impl().do_PushToMainThread(function, pUserData);
	}
};
