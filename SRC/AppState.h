#pragma once
#include <atomic>

/*
If g_running is false, all processes, loops, and threads should exit.
After that, application must return g_retcode.

However, a screen reader is a program that must be robust against exceptions and continue running.
Hopefully, these flags will only be used when we disable the screen reader.
*/
inline std::atomic<bool> g_running{false};
inline int g_retcode{0};
