#pragma once
#include "Engine.h"
#include <string>
#include <speech-dispatcher/libspeechd.h>

namespace Sral {
	class SpeechDispatcher final : public Engine {
	public:
		auto Speak(const std::string& text, bool interrupt) -> bool override;

		auto Braille(const std::string& text) -> bool override;

		auto IsSpeaking() -> bool override;

		auto SetParameter(int param, const void* value) -> bool override;
		auto GetParameter(int param, void* value) -> bool override;


		auto StopSpeech() -> bool override;
		auto PauseSpeech() -> bool override;
		auto ResumeSpeech() -> bool override;
		auto GetActive() -> bool override;
		auto Initialize() -> bool override;
		auto Uninitialize() -> bool override;

	private:
		SPDConnection* speech = nullptr;
		bool enableSpelling = false;
		bool brailleInitialized = false;

		SPDVoice** m_voiceList{nullptr};
		int m_voiceCount{0};
		int m_voiceIndex{0};
		auto SetVoiceIndex() -> int;
		inline void ClearVoiceList() {
			if (m_voiceList) {
				free_spd_voices(m_voiceList);
				m_voiceList = nullptr;
			}
		m_voiceCount = 0;
		}

		inline void RefreshVoiceList() {
			ClearVoiceList();
			if (speech == nullptr) return;
			m_voiceList = spd_list_synthesis_voices(speech);
			if (!m_voiceList) return;
			for (; m_voiceList[m_voiceCount] != nullptr; ++m_voiceCount);
		}

		static void SpeechNotificationCallback(size_t msg_id, size_t client_id, SPDNotificationType type);
	};
}

