// Event listener trait
module;
#include <utility>
export module Traits.EventListener;
import Core.Device;

/*
A event listener is a platform-specific trait that processes events and converts them to a common screen reader's
IEvent.
*/

export using ThreadFunction = void (*)(void*);
export template <typename Derived> class TEventListener {
public:
	void ListenDevice(this auto&& self, EDeviceType type, bool listen = true) {
		return std::forward<decltype(self)>(self).do_ListenDevice(type, listen);
	}

	void PushToMainThread(this auto&& self, ThreadFunction function, void* pUserData) {
		return std::forward<decltype(self)>(self).do_PushToMainThread(function, pUserData);
	}
};
