#pragma once

struct SRect {
	int x, y, width, height;

	[[nodiscard]] inline bool Contains(int px, int py) const {
		return px >= x && px <= x + width && py >= y && py <= y + height;
	}
};

