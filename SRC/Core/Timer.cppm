// Some inline utilities that make working with STD/other things easier.
module;
#include <chrono>

export module Core.Timer;

export class CTimer final {
	std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;

public:
	explicit CTimer() { Restart(); }

	[[nodiscard]] auto Elapsed() -> uint64_t {
		auto now = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime).count();
	}

	void Restart() { m_startTime = std::chrono::high_resolution_clock::now(); }
};
