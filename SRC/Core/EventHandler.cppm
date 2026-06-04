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

// Handling events of different types.
module;
#include "Environment.h"

#include <memory>
#include <thread>
export module Core.EventHandler;
import Core.Event;
import Core.EventQueue;
import Core.Logger;
import Core.Singleton;
#if SR_LINUX
import Platforms.Linux.EventListener;
#endif

export class EventHandler final : TModule<"EventHandler">, public TSingleton<EventHandler> {
	/*
	A listener is a platform-specific trait that processes events and converts them to a common screen reader's
	Event. Then this event handler then queries the queue of these events.
	*/
	CEventListener m_listener;
	EventQueue& m_eventQueue;

	std::jthread m_thread;

public:
	explicit EventHandler();
	~EventHandler() { Stop(); }

	void Handle(CEvent&& event);

	void Start();
	void Stop();
	inline auto GetListener() -> CEventListener* { return &m_listener; }
};
