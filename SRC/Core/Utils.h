// Some inline utilities that make working with STD/other things easier.
#pragma once
#include <chrono>

class CTimer final {
	std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;

public:
	explicit CTimer() { Restart(); }

	[[nodiscard]] inline auto Elapsed() -> uint64_t {
		auto now = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime).count();
	}

	inline void Restart() { m_startTime = std::chrono::high_resolution_clock::now(); }
};
