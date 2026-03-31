module;
#include <string_view>
export module Core.Text;

export enum class ETextGranularity : unsigned char { CHARACTER, WORD, SENTENCE, LINE, PARAGRAPH };

export struct STextRange final {
	int start{0};
	int end{0};
	[[maybe_unused]] ETextGranularity granularity{ETextGranularity::CHARACTER};
	std::string_view text{""};
};
