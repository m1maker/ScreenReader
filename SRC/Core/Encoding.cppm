module;
#include <algorithm>
#include <array>
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

export struct SUtf8EncodeResult final {
	char bytes[4]{};
	size_t size{0};
};

export [[nodiscard]] inline auto EncodeUtf8(uint32_t code_point) -> SUtf8EncodeResult {
	SUtf8EncodeResult result;

	if (code_point < 0x80) {
		result.bytes[0] = static_cast<char>(code_point);
		result.size = 1;
	}
	else if (code_point < 0x800) {
		result.bytes[0] = static_cast<char>(0xC0 | ((code_point >> 6) & 0x1F));
		result.bytes[1] = static_cast<char>(0x80 | (code_point & 0x3F));
		result.size = 2;
	}
	else if (code_point < 0x10000) {
		if (code_point >= 0xD800 && code_point <= 0xDFFF) {
			return EncodeUtf8(0xFFFD);
		}

		result.bytes[0] = static_cast<char>(0xE0 | ((code_point >> 12) & 0x0F));
		result.bytes[1] = static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
		result.bytes[2] = static_cast<char>(0x80 | (code_point & 0x3F));
		result.size = 3;
	}
	else if (code_point <= 0x10FFFF) {
		result.bytes[0] = static_cast<char>(0xF0 | ((code_point >> 18) & 0x07));
		result.bytes[1] = static_cast<char>(0x80 | ((code_point >> 12) & 0x3F));
		result.bytes[2] = static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
		result.bytes[3] = static_cast<char>(0x80 | (code_point & 0x3F));
		result.size = 4;
	}
	else {
		return EncodeUtf8(0xFFFD);
	}

	return result;
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

namespace PunctuationNames {
struct SPunctuationEntry final {
	char32_t character{0};
	std::string_view name{"unknown character"};
};

static constexpr std::array<SPunctuationEntry, 138> cPunctuationMap = {{// ASCII punctuation
	{U'!', "bang"},
	{U'"', "quote"},
	{U'#', "number"},
	{U'$', "dollar"},
	{U'%', "percent"},
	{U'&', "and"},
	{U'\'', "tick"},
	{U'(', "left paren"},
	{U')', "right paren"},
	{U'*', "star"},
	{U'+', "plus"},
	{U',', "comma"},
	{U'-', "dash"},
	{U'.', "dot"},
	{U'/', "slash"},
	{U':', "colon"},
	{U';', "semi"},
	{U'<', "less"},
	{U'=', "equals"},
	{U'>', "greater"},
	{U'?', "question"},
	{U'@', "at"},
	{U'[', "left bracket"},
	{U'\\', "backslash"},
	{U']', "right bracket"},
	{U'^', "caret"},
	{U'_', "underscore"},
	{U'`', "grave"},
	{U'{', "left brace"},
	{U'|', "bar"},
	{U'}', "right brace"},
	{U'~', "tilde"},

	// General punctuation
	{U'‐', "dash"},
	{U'‑', "non-breaking dash"},
	{U'‒', "figure dash"},
	{U'–', "en dash"},
	{U'—', "em dash"},
	{U'―', "horizontal bar"},
	{U'‖', "double vertical line"},
	{U'‘', "left single quote"},
	{U'’', "right single quote"},
	{U'‚', "single low-9 quote"},
	{U'‛', "single high-reversed-9 quote"},
	{U'“', "left double quote"},
	{U'”', "right double quote"},
	{U'„', "double low-9 quote"},
	{U'‟', "double high-reversed-9 quote"},
	{U'†', "dagger"},
	{U'‡', "double dagger"},
	{U'•', "bullet"},
	{U'‥', "two dot leader"},
	{U'…', "ellipsis"},
	{U'‧', "hyphenation point"},
	{U'‰', "per mille"},
	{U'‱', "per ten thousand"},
	{U'′', "prime"},
	{U'″', "double prime"},
	{U'‴', "triple prime"},
	{U'‵', "reversed prime"},
	{U'‶', "reversed double prime"},
	{U'‷', "reversed triple prime"},
	{U'‸', "caret"},
	{U'※', "reference"},
	{U'‼', "double bang"},
	{U'‽', "interrobang"},
	{U'‾', "overline"},
	{U'‿', "undertie"},
	{U'⁀', "character tie"},
	{U'⁁', "caret insertion point"},
	{U'⁂', "asterism"},
	{U'⁃', "hyphen bullet"},
	{U'⁄', "fraction slash"},
	{U'⁅', "left bracket with quill"},
	{U'⁆', "right bracket with quill"},
	{U'⁇', "double question"},
	{U'⁈', "question bang"},
	{U'⁉', "bang question"},
	{U'⁊', "tironian sign et"},

	// Dashes and other common punctuation
	{U'—', "em dash"},
	{U'–', "en dash"},
	{U'〜', "wave dash"},
	{U'〰', "wavy dash"},

	// Curly quotes and dashes
	{U'«', "left-pointing double angle quote"},
	{U'»', "right-pointing double angle quote"},
	{U'‹', "single left-pointing angle quote"},
	{U'›', "single right-pointing angle quote"},

	// Braces and brackets variations
	{U'〈', "left-pointing angle bracket"},
	{U'〉', "right-pointing angle bracket"},
	{U'◊', "lozenge"},
	{U'✓', "check mark"},
	{U'✗', "ballot x"},
	{U'✘', "heavy ballot x"},

	// Mathematical operators as punctuation
	{U'÷', "division"},
	{U'×', "multiplication"},
	{U'±', "plus-minus"},
	{U'∓', "minus-or-plus"},
	{U'∑', "summation"},
	{U'∏', "product"},
	{U'∫', "integral"},
	{U'√', "square root"},
	{U'∞', "infinity"},
	{U'≈', "almost equal to"},
	{U'≠', "not equal to"},
	{U'≡', "identical to"},
	{U'≤', "less-than or equal to"},
	{U'≥', "greater-than or equal to"},
	{U'⊂', "subset of"},
	{U'⊃', "superset of"},
	{U'⊆', "subset of or equal to"},
	{U'⊇', "superset of or equal to"},
	{U'⊕', "circled plus"},
	{U'⊗', "circled times"},
	{U'⊥', "up tack"},
	{U'∠', "angle"},

	// Currency symbols
	{U'€', "euro"},
	{U'£', "pound"},
	{U'¥', "yen"},
	{U'¢', "cent"},
	{U'₹', "rupee"},
	{U'₽', "ruble"},
	{U'₿', "bitcoin"},

	// Common emoji punctuation
	{U'❤', "heart"},
	{U'★', "star"},
	{U'☆', "white star"},
	{U'☀', "sun"},
	{U'☁', "cloud"},
	{U'☂', "umbrella"},
	{U'☃', "snowman"},
	{U'☄', "comet"},
	{U'🔥', "fire"},

	// Arrows
	{U'←', "left arrow"},
	{U'↑', "up arrow"},
	{U'→', "right arrow"},
	{U'↓', "down arrow"},
	{U'↔', "left right arrow"},
	{U'↕', "up down arrow"},
	{U'◀', "black left-pointing triangle"},
	{U'▶', "black right-pointing triangle"}}};

[[nodiscard]] constexpr auto FindPunctuationName(char32_t c) -> std::string_view {
	if (c < 128) {
		for (const auto& entry : cPunctuationMap) {
			if (entry.character == c) {
				return entry.name;
			}
		}
	}
	else {
		for (const auto& entry : cPunctuationMap) {
			if (entry.character == c) {
				return entry.name;
			}
		}
	}
	return "unknown character";
}

} // namespace PunctuationNames

export [[nodiscard]] constexpr auto PunctuationToName(char32_t c) -> std::string_view {
	return PunctuationNames::FindPunctuationName(c);
}

export [[nodiscard]] constexpr auto PunctuationToName(char c) -> std::string_view {
	return PunctuationToName(static_cast<char32_t>(c));
}
