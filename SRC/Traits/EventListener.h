// Event listener trait
#pragma once

/*
A event listener is a platform-specific trait that processes events and converts them to a common screen reader's
IEvent.
*/

using ThreadFunction = void (*)(void*);
template <typename T> struct EventListenerTrait final {};
