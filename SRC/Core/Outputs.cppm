module;
#include <cstdio>
#include <variant>
export module Core.Outputs;
export import Core.Outputs.Speech;

export using OutputVariant = std::variant<std::monostate, COutputSpeech>;

export constexpr size_t cOutputCount = 1;
