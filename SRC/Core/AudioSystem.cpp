module;
#include <mutex>
#include <stop_token>
#include <thread>
module Core.AudioSystem;
import Core.Audio;

void AudioSystem::Start() {
	m_parameters = SAudioParameters::GetDefault();
	auto result = Initialize(m_parameters);
	if (!result) {
		Log(ERROR, "Failed to initialize audio engine");
		return;
	}

	m_bytesPerSample = TAudioFormatSize</*m_parameters.format*/ EAudioFormat::S16>::value;
	m_bytesPerFrame = m_parameters.channels * m_bytesPerSample;

	m_thread = std::jthread([this](const std::stop_token& stop_token) -> void {
		while (!stop_token.stop_requested()) {
			std::unique_lock lock(m_mutex);
			m_cv.wait(lock, [this, stop_token] -> bool { return !m_queue.empty() || stop_token.stop_requested(); });
			if (stop_token.stop_requested())
				break;

			auto data = std::move(m_queue.front());
			m_queue.pop();

			auto frames = data.size() / m_bytesPerFrame;
			auto result = Write(data.data(), frames);
			if (!result) {
			}
		}
	});
}

void AudioSystem::Stop() {
	m_cv.notify_all();
	m_thread.request_stop();
}

void AudioSystem::PushData(const AudioData&& data) {
	std::scoped_lock _(m_mutex);
	m_queue.push(std::move(data));
	m_cv.notify_one();
}
