module;
#include <string>
export module Core.Text;

export enum class ETextGranularity : unsigned char { CHARACTER, WORD, SENTENCE, LINE, PARAGRAPH };

export template <class T> struct STextRange final {
	int start{0};
	int end{0};
	[[maybe_unused]] ETextGranularity granularity{ETextGranularity::CHARACTER};
	T text{""};
};

export template <> struct STextRange<void> final {
	int start{0};
	int end{0};
	[[maybe_unused]] ETextGranularity granularity{ETextGranularity::CHARACTER};
};

export template <typename T>
concept Textable = requires(T t) {
	{ t.text };
};
