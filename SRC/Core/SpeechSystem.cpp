module;
#include <mutex>
#include <stop_token>
#include <thread>
module Core.SpeechSystem;
import Core.App;
import Core.AudioSystem;
import Core.Speech;

void SpeechSystem::ApplySpeechParameters(SpeechParameters parameters) {
	m_rate = parameters.rate;
	m_volume = parameters.volume;
	m_pitch = parameters.pitch;
	m_pitchRange = parameters.pitch_range;
	m_ssml = parameters.ssml;
}

void SpeechSystem::Start() {
	m_thread = std::jthread([this](const std::stop_token& stop_token) -> void {
		while (!stop_token.stop_requested()) {
			std::unique_lock lock(m_mutex);
			m_cv.wait(lock, [this, stop_token] -> bool { return !m_queue.empty() || stop_token.stop_requested(); });
			if (stop_token.stop_requested())
				break;

			auto message = std::move(m_queue.front());
			m_queue.pop();
			lock.unlock();
			if (message.interrupt)
				EngineStop();

			auto set_parameter_result = EngineSetParameter(ESpeechEngineParameter::RATE, message.rate);
			set_parameter_result = EngineSetParameter(ESpeechEngineParameter::VOLUME, message.volume);
			set_parameter_result = EngineSetParameter(ESpeechEngineParameter::PITCH, message.pitch);
			set_parameter_result = EngineSetParameter(ESpeechEngineParameter::PITCH_RANGE, message.pitch_range);
			set_parameter_result = EngineSetParameter(ESpeechEngineParameter::SSML, message.ssml);
			if (!set_parameter_result) {
				Log(WARNING,
					"Failed to set one or more speech engine parameters. {}",
					SpeechEngineErrorToString(set_parameter_result.error()));
			}
			auto result = EngineSpeak(message.message);
			if (!result) {
			}
		}
	});
}

void SpeechSystem::Stop() {
	m_thread.request_stop();
	m_cv.notify_all();
}

void SpeechSystem::Speak(std::string_view message, bool interrupt) {
	if (message.empty()) [[unlikely]]
		return;

	ApplySpeechParameters(ScreenReaderApp::GetInstance().GetSettings().speech);

	SSpeechMessage queued_message{.interrupt = interrupt,
		.ssml = m_ssml,
		.rate = m_rate,
		.volume = m_volume,
		.pitch = m_pitch,
		.pitch_range = m_pitchRange};
	queued_message.message = std::pmr::string(message, m_pool);

	m_shouldAbort.clear(std::memory_order_release);

	{
		std::scoped_lock _(m_mutex);
		m_queue.push(std::move(queued_message));
	}
	m_cv.notify_all();
}

void SpeechSystem::Interrupt() {
	m_shouldAbort.test_and_set(std::memory_order_acquire);
	AudioSystem::GetInstance().Stop(0);
	SSpeechMessage message{.interrupt = true};
	std::scoped_lock _(m_mutex);
	m_queue.push(std::move(message));
}