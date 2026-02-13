#pragma once
#include <string>

enum class ETextGranularity : unsigned char {
	CHARACTER,
	WORD,
	SENTENCE,
	LINE,
	PARAGRAPH
};

template<class T>
struct STextRange final {
	int start{0};
	int end{0};
	T text{""};
};

template<>
struct STextRange<void> final {
	int start{0};
	int end{0};
};

template<typename T>
concept Textable = requires(T t) {
	{ t.text };
};
