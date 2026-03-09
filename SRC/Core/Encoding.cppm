module;
#include <cstdint>
#include <string>
#include <string_view>
export module Core.Encoding;

export struct SUtf8Result final {
	uint32_t code_point{0};
	size_t size{0};
};

export [[nodiscard]] inline auto DecodeUtf8(const char* pointer) -> SUtf8Result {
	auto c = static_cast<unsigned char>(*pointer);
	if (c < 0x80)
		return {c, 1};
	if ((c & 0xE0) == 0xC0) {
		return {static_cast<uint32_t>(((c & 0x1F) << 6) | (static_cast<unsigned char>(pointer[1]) & 0x3F)), 2};
	}

	if ((c & 0xF0) == 0xE0) {
		return {static_cast<uint32_t>(((c & 0x0F) << 12) | ((static_cast<unsigned char>(pointer[1]) & 0x3F) << 6) |
					(static_cast<unsigned char>(pointer[2]) & 0x3F)),
			3};
	}

	if ((c & 0xF8) == 0xF0) {
		return {static_cast<uint32_t>(((c & 0x07) << 18) | ((static_cast<unsigned char>(pointer[1]) & 0x3F) << 12) |
					((static_cast<unsigned char>(pointer[2]) & 0x3F) << 6) |
					(static_cast<unsigned char>(pointer[3]) & 0x3F)),
			4};
	}

	return {0xFFFD, 1};
}

export class CUtf8Iterator final {
	const char* m_pointer{nullptr};

public:
	explicit CUtf8Iterator(const char* pointer) : m_pointer(pointer) {}

	[[nodiscard]] auto operator*() const -> uint32_t { return DecodeUtf8(m_pointer).code_point; }

	[[nodiscard]] auto operator++() -> CUtf8Iterator& {
		m_pointer += DecodeUtf8(m_pointer).size;
		return *this;
	}

	[[nodiscard]] auto operator!=(const CUtf8Iterator& other) const -> bool { return m_pointer != other.m_pointer; }
};

export class CUtf8View final {
	const char* m_begin{nullptr};
	const char* m_end{nullptr};

public:
	explicit CUtf8View(const char* str, size_t len) : m_begin(str), m_end(str + len) {}
	explicit CUtf8View(const std::string& s) : m_begin(s.data()), m_end(s.data() + s.size()) {}
	explicit CUtf8View(std::string_view s) : m_begin(s.data()), m_end(s.data() + s.size()) {}


	[[nodiscard]] auto begin() const -> CUtf8Iterator { return CUtf8Iterator(m_begin); }
	[[nodiscard]] auto end() const -> CUtf8Iterator { return CUtf8Iterator(m_end); }
};
