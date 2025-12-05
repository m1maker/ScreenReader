#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>

namespace Sral {
	class Engine {
	public:
		Engine();
		virtual ~Engine();			
		virtual auto Speak(const std::string& text, bool interrupt) -> bool;
		virtual auto Braille(const std::string& text) -> bool;

		virtual auto SpeakToMemory(const std::string& text, uint64_t* buffer_size, int* channels, int* sample_rate, int* bits_per_sample) -> void*;
		virtual auto StopSpeech() -> bool;
		virtual auto PauseSpeech() -> bool;
		virtual auto ResumeSpeech() -> bool;
		virtual auto IsSpeaking() -> bool;
		virtual auto GetNumber() -> int;
		virtual auto GetActive() -> bool;
		virtual auto GetFeatures() -> int;
		virtual auto Initialize() -> bool;
		virtual auto Uninitialize() -> bool;
		virtual auto SetParameter(int param, const void* value) -> bool;
		virtual auto GetParameter(int param, void* value) -> bool;

		bool paused{};
	protected:
		std::vector<char*> m_strings;

		inline auto AddString(const char* str) -> const char* {
			if (!str) return nullptr;

			size_t len = strlen(str) + 1;
			char* cString = new char[len];
			strcpy(cString, str);
			m_strings.push_back(cString);
			return cString;
		}

		inline void ReleaseAllStrings() {
			for (auto str : m_strings) {
				delete[] str;
			}
			m_strings.clear();
		}
	};
}
