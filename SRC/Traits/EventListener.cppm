/*
* This file is part of the Screen Reader project.
*
* Copyright (C) 2025-2026  Georgiy Bondarenko (M_maker / m1maker) <georgijbondarenko248@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
