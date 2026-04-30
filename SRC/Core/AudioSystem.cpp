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
				Log(ERROR, "Failed to write the audio data. {}", AudioEngineErrorToString(result.error()));
			}
		}
	});
}

void AudioSystem::Stop(void) {
	m_thread.request_stop();
	m_cv.notify_all();
	Uninitialize();
}

void AudioSystem::PushData(unsigned char channel, const AudioDataVector& data) {
	if (data.empty())
		return;

	m_channelsShouldStop[channel].store(0);

	std::scoped_lock _(m_mutex);

	auto chunk = std::move(m_partialChunks[channel]);
	if (chunk.data.empty()) {
		chunk.channel = channel;
		chunk.data.fill(0);
	}
	auto sample = m_partialChunkSamples[channel];

	for (size_t i = 0; i < data.size(); ++i) {
		chunk.data[sample++] = data[i];

		if (sample >= cAudioChunkSize) {
			m_queue.push(std::move(chunk));
			sample = 0;
			chunk = {};
			chunk.channel = channel;
			chunk.data.fill(0);
		}
	}

	m_partialChunks[channel] = std::move(chunk);
	m_partialChunkSamples[channel] = sample;

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