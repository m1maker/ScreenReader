#include <cstdint>
#include <linux/input.h>
#include <linux/uinput.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <stdexcept>

class CUinputDevice final {
	int m_devFd{-1};
	int m_uinputFd{-1};

	void SetupVirtualDevice();
public:

	explicit CUinputDevice(int device_to_grab);

	~CUinputDevice();

	void Grab(int device_to_grab);
	void Post(uint16_t type, uint16_t code, int32_t value);

	[[nodiscard]] inline auto GetFd() const -> int { return m_devFd; }

};