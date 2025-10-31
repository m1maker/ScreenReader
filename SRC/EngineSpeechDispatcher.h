#pragma once
#include "Engine.h"
#include <string>
#include <speech-dispatcher/libspeechd.h>

namespace Sral {
	class SpeechDispatcher final : public Engine {
	public:
		bool Speak(const std::string& text, bool interrupt)override;

		bool Braille(const std::string& text)override;

		bool IsSpeaking()override;

		bool SetParameter(int param, const void* value)override;
		bool GetParameter(int param, void* value) override;


		bool StopSpeech()override;
		bool PauseSpeech()override;
		bool ResumeSpeech()override;
		bool GetActive()override;
		bool Initialize()override;
		bool Uninitialize()override;

	private:
		SPDConnection* speech = nullptr;
		bool enableSpelling = false;
		bool brailleInitialized = false;

		SPDVoice** m_voiceList{nullptr};
		int m_voiceCount{0};
		int m_voiceIndex{0};
		int SetVoiceIndex();
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

