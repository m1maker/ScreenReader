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

			auto chunk = std::move(m_queue.front());
			m_queue.pop();
			lock.unlock();
			if (m_channelsShouldStop[chunk.channel].load())
				continue;
			auto frames = chunk.data.size() / m_bytesPerFrame * 2;
			auto result = Write(chunk.data.data(), frames);
			if (!result) {
			}
		}
	});
}

void AudioSystem::Stop(void) {
	m_cv.notify_all();
	m_thread.request_stop();
	Uninitialize();
}

void AudioSystem::PushData(unsigned char channel, const AudioDataVector& data) {
	if (data.empty()) [[unlikely]]
		return;

	m_channelsShouldStop[channel].store(0);
	size_t sample{0};
	SAudioChunk chunk{.channel = channel};
	for (size_t i = 0; i < data.size(); ++i) {
		std::scoped_lock _(m_mutex);

		if (sample < cAudioChunkSize) {
			chunk.data[sample] = data[i];
			++sample;
		}
		else {
			m_queue.push(chunk);
			sample = 0;
			chunk.data = {};
			chunk.data[sample] = data[i];
			++sample;
		}
	}
	m_cv.notify_all();
}

void AudioSystem::Stop(unsigned char channel) {
	m_channelsShouldStop[channel].store(1);
	std::scoped_lock _(m_mutex);

	AudioDataQueue new_queue;
	while (!m_queue.empty()) {
		auto chunk = std::move(m_queue.front());
		if (chunk.channel != channel) {
			new_queue.push(std::move(chunk));
		}
		m_queue.pop();
	}
	m_queue = std::move(new_queue);
}