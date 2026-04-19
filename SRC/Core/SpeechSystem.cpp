module;
#include <mutex>
#include <stop_token>
#include <thread>
module Core.SpeechSystem;
import Core.AudioSystem;
import Core.Speech;

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

void SpeechSystem::Speak(std::string_view message, bool interrupt, bool ssml) {
	if (message.empty()) [[unlikely]]
		return;

	SSpeechMessage queued_message{.message = message, .interrupt = interrupt, .ssml = ssml};
	{
		std::scoped_lock _(m_mutex);
		m_queue.push(std::move(queued_message));
	}
	m_cv.notify_all();
}

void SpeechSystem::Interrupt() {
	SSpeechMessage message{.interrupt = true};
	std::scoped_lock _(m_mutex);
	m_queue.push(std::move(message));
	AudioSystem::GetInstance().Stop(0);
}