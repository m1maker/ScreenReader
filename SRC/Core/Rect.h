#pragma once

struct SRect {
	int x, y, width, height;

	[[nodiscard]] inline auto Contains(int px, int py) const -> bool {
		return px >= x && px <= x + width && py >= y && py <= y + height;
	}
};
