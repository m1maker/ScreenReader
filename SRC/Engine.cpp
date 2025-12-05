#include "Engine.h"
#include <cstddef>

namespace Sral {
	Engine::Engine() = default;

	Engine::~Engine() {
		ReleaseAllStrings();
		Uninitialize();
	}

	auto Engine::Braille(const std::string& text) -> bool {
		return false;
	}

	auto Engine::GetActive() -> bool {
		return false;
	}

	auto Engine::GetFeatures() -> int {
		return 0;
	}

	auto Engine::GetNumber() -> int {
		return 0;
	}

	auto Engine::GetParameter(int parameter, void* value) -> bool {
		(void)parameter;
		(void)value;
		return false;
	}

	auto Engine::Initialize() -> bool {
		return false;
	}

	auto Engine::IsSpeaking() -> bool {
		return false;
	}

	auto Engine::PauseSpeech() -> bool {
		return false;
	}

	auto Engine::ResumeSpeech() -> bool {
		return false;
	}

	auto Engine::SetParameter(int parameter, const void* value) -> bool {
		(void)parameter;
		(void)value;
		return false;
	}

	auto Engine::Speak(const std::string& text, bool interrupt) -> bool {
		(void)text;
		(void)interrupt;
		return false;
	}

	auto Engine::SpeakToMemory(const std::string& text, uint64_t* buffer_size, int* channels, int* sample_rate, int* bits_per_sample) -> void* {
		(void)text;
		(void)buffer_size;
		(void)channels;
		(void)sample_rate;
		(void)bits_per_sample;
		return nullptr;
	}

	auto Engine::StopSpeech() -> bool {
		return false;
	}

	auto Engine::Uninitialize() -> bool {
		return false;
	}
}
