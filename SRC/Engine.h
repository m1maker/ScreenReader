#pragma once
#include <vector>
#include <string>
#include <stdint.h>
#include <cstring>

namespace Sral {
	class Engine {
	public:
		Engine();
		virtual ~Engine();			
		virtual bool Speak(const std::string& text, bool interrupt);
		virtual bool Braille(const std::string& text);

		virtual void* SpeakToMemory(const std::string& text, uint64_t* buffer_size, int* channels, int* sample_rate, int* bits_per_sample);
		virtual bool StopSpeech();
		virtual bool PauseSpeech();
		virtual bool ResumeSpeech();
		virtual bool IsSpeaking();
		virtual int GetNumber();
		virtual bool GetActive();
		virtual int GetFeatures();
		virtual bool Initialize();
		virtual bool Uninitialize();
		virtual bool SetParameter(int param, const void* value);
		virtual bool GetParameter(int param, void* value);

		bool paused;
	protected:
		std::vector<char*> m_strings;

		inline const char* AddString(const char* str) {
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
