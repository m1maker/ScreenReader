#pragma once
#include <string>

enum class ETextGranularity : unsigned char {
	CHARACTER,
	WORD,
	SENTENCE,
	LINE,
	PARAGRAPH
};

struct STextRange final {
	int start{0};
	int end{0};
	std::string text{""};

	STextRange() = default;
};

