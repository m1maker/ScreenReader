export module Core.Rect;
export struct SRect final {
	int x, y, width, height;

	[[nodiscard]] inline auto Contains(int px, int py) const -> bool {
		return px >= x && px <= x + width && py >= y && py <= y + height;
	}
};
