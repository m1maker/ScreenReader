/*
 * This file is part of the Screen Reader project.
 *
 * Copyright (C) 2025-2026  Georgiy Bondarenko (M_maker / m1maker) <georgijbondarenko248@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
	char32_t character;
	std::string_view name;
};

constexpr std::array cPunctuationMap = { // ASCII punctuation
	SPunctuationEntry{U'!', "bang"},
	SPunctuationEntry{U'"', "quote"},
	SPunctuationEntry{U'#', "number"},
	SPunctuationEntry{U'$', "dollar"},
	SPunctuationEntry{U'%', "percent"},
	SPunctuationEntry{U'&', "and"},
	SPunctuationEntry{U'\'', "tick"},
	SPunctuationEntry{U'(', "left paren"},
	SPunctuationEntry{U')', "right paren"},
	SPunctuationEntry{U'*', "star"},
	SPunctuationEntry{U'+', "plus"},
	SPunctuationEntry{U',', "comma"},
	SPunctuationEntry{U'-', "dash"},
	SPunctuationEntry{U'.', "dot"},
	SPunctuationEntry{U'/', "slash"},
	SPunctuationEntry{U':', "colon"},
	SPunctuationEntry{U';', "semi"},
	SPunctuationEntry{U'<', "less"},
	SPunctuationEntry{U'=', "equals"},
	SPunctuationEntry{U'>', "greater"},
	SPunctuationEntry{U'?', "question"},
	SPunctuationEntry{U'@', "at"},
	SPunctuationEntry{U'[', "left bracket"},
	SPunctuationEntry{U'\\', "backslash"},
	SPunctuationEntry{U']', "right bracket"},
	SPunctuationEntry{U'^', "caret"},
	SPunctuationEntry{U'_', "underscore"},
	SPunctuationEntry{U'`', "grave"},
	SPunctuationEntry{U'{', "left brace"},
	SPunctuationEntry{U'|', "bar"},
	SPunctuationEntry{U'}', "right brace"},
	SPunctuationEntry{U'~', "tilde"},

	// General punctuation
	SPunctuationEntry{U'‐', "dash"},
	SPunctuationEntry{U'‑', "non-breaking dash"},
	SPunctuationEntry{U'‒', "figure dash"},
	SPunctuationEntry{U'–', "en dash"},
	SPunctuationEntry{U'—', "em dash"},
	SPunctuationEntry{U'―', "horizontal bar"},
	SPunctuationEntry{U'‖', "double vertical line"},
	SPunctuationEntry{U'‘', "left single quote"},
	SPunctuationEntry{U'’', "right single quote"},
	SPunctuationEntry{U'‚', "single low-9 quote"},
	SPunctuationEntry{U'‛', "single high-reversed-9 quote"},
	SPunctuationEntry{U'“', "left double quote"},
	SPunctuationEntry{U'”', "right double quote"},
	SPunctuationEntry{U'„', "double low-9 quote"},
	SPunctuationEntry{U'‟', "double high-reversed-9 quote"},
	SPunctuationEntry{U'†', "dagger"},
	SPunctuationEntry{U'‡', "double dagger"},
	SPunctuationEntry{U'•', "bullet"},
	SPunctuationEntry{U'‥', "two dot leader"},
	SPunctuationEntry{U'…', "ellipsis"},
	SPunctuationEntry{U'‧', "hyphenation point"},
	SPunctuationEntry{U'‰', "per mille"},
	SPunctuationEntry{U'‱', "per ten thousand"},
	SPunctuationEntry{U'′', "prime"},
	SPunctuationEntry{U'″', "double prime"},
	SPunctuationEntry{U'‴', "triple prime"},
	SPunctuationEntry{U'‵', "reversed prime"},
	SPunctuationEntry{U'‶', "reversed double prime"},
	SPunctuationEntry{U'‷', "reversed triple prime"},
	SPunctuationEntry{U'‸', "caret"},
	SPunctuationEntry{U'※', "reference"},
	SPunctuationEntry{U'‼', "double bang"},
	SPunctuationEntry{U'‽', "interrobang"},
	SPunctuationEntry{U'‾', "overline"},
	SPunctuationEntry{U'‿', "undertie"},
	SPunctuationEntry{U'⁀', "character tie"},
	SPunctuationEntry{U'⁁', "caret insertion point"},
	SPunctuationEntry{U'⁂', "asterism"},
	SPunctuationEntry{U'⁃', "hyphen bullet"},
	SPunctuationEntry{U'⁄', "fraction slash"},
	SPunctuationEntry{U'⁅', "left bracket with quill"},
	SPunctuationEntry{U'⁆', "right bracket with quill"},
	SPunctuationEntry{U'⁇', "double question"},
	SPunctuationEntry{U'⁈', "question bang"},
	SPunctuationEntry{U'⁉', "bang question"},
	SPunctuationEntry{U'⁊', "tironian sign et"},

	// Dashes and other common punctuation
	SPunctuationEntry{U'—', "em dash"},
	SPunctuationEntry{U'–', "en dash"},
	SPunctuationEntry{U'〜', "wave dash"},
	SPunctuationEntry{U'〰', "wavy dash"},

	// Curly quotes and dashes
	SPunctuationEntry{U'«', "left-pointing double angle quote"},
	SPunctuationEntry{U'»', "right-pointing double angle quote"},
	SPunctuationEntry{U'‹', "single left-pointing angle quote"},
	SPunctuationEntry{U'›', "single right-pointing angle quote"},

	// Braces and brackets variations
	SPunctuationEntry{U'〈', "left-pointing angle bracket"},
	SPunctuationEntry{U'〉', "right-pointing angle bracket"},
	SPunctuationEntry{U'◊', "lozenge"},
	SPunctuationEntry{U'✓', "check mark"},
	SPunctuationEntry{U'✗', "ballot x"},
	SPunctuationEntry{U'✘', "heavy ballot x"},

	// Mathematical operators as punctuation
	SPunctuationEntry{U'÷', "division"},
	SPunctuationEntry{U'×', "multiplication"},
	SPunctuationEntry{U'±', "plus-minus"},
	SPunctuationEntry{U'∓', "minus-or-plus"},
	SPunctuationEntry{U'∑', "summation"},
	SPunctuationEntry{U'∏', "product"},
	SPunctuationEntry{U'∫', "integral"},
	SPunctuationEntry{U'√', "square root"},
	SPunctuationEntry{U'∞', "infinity"},
	SPunctuationEntry{U'≈', "almost equal to"},
	SPunctuationEntry{U'≠', "not equal to"},
	SPunctuationEntry{U'≡', "identical to"},
	SPunctuationEntry{U'≤', "less-than or equal to"},
	SPunctuationEntry{U'≥', "greater-than or equal to"},
	SPunctuationEntry{U'⊂', "subset of"},
	SPunctuationEntry{U'⊃', "superset of"},
	SPunctuationEntry{U'⊆', "subset of or equal to"},
	SPunctuationEntry{U'⊇', "superset of or equal to"},
	SPunctuationEntry{U'⊕', "circled plus"},
	SPunctuationEntry{U'⊗', "circled times"},
	SPunctuationEntry{U'⊥', "up tack"},
	SPunctuationEntry{U'∠', "angle"},

	// Currency symbols
	SPunctuationEntry{U'€', "euro"},
	SPunctuationEntry{U'£', "pound"},
	SPunctuationEntry{U'¥', "yen"},
	SPunctuationEntry{U'¢', "cent"},
	SPunctuationEntry{U'₹', "rupee"},
	SPunctuationEntry{U'₽', "ruble"},
	SPunctuationEntry{U'₿', "bitcoin"},

	// Common emoji punctuation
	SPunctuationEntry{U'❤', "heart"},
	SPunctuationEntry{U'★', "star"},
	SPunctuationEntry{U'☆', "white star"},
	SPunctuationEntry{U'☀', "sun"},
	SPunctuationEntry{U'☁', "cloud"},
	SPunctuationEntry{U'☂', "umbrella"},
	SPunctuationEntry{U'☃', "snowman"},
	SPunctuationEntry{U'☄', "comet"},
	SPunctuationEntry{U'🔥', "fire"},

	// Arrows
	SPunctuationEntry{U'←', "left arrow"},
	SPunctuationEntry{U'↑', "up arrow"},
	SPunctuationEntry{U'→', "right arrow"},
	SPunctuationEntry{U'↓', "down arrow"},
	SPunctuationEntry{U'↔', "left right arrow"},
	SPunctuationEntry{U'↕', "up down arrow"},
	SPunctuationEntry{U'◀', "black left-pointing triangle"},
	SPunctuationEntry{U'▶', "black right-pointing triangle"}};

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
