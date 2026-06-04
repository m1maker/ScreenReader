module;
#include <algorithm>
#include <cstring>
#include <mutex>
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
}

void AudioSystem::Stop(void) {
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

void AudioSystem::Read(signed short int* buffer, unsigned long long frames) {
	memset(buffer, 0, frames * m_parameters.channels * sizeof(signed short int));

	std::scoped_lock _(m_mutex);

	size_t frames_written{0};
	size_t buffer_offset{0};

	while (frames_written < frames && !m_queue.empty()) {
		auto& chunk = m_queue.front();

		auto remaining_in_chunk = cAudioChunkSize - chunk.read_offset;
		auto frames_to_copy = std::min(static_cast<unsigned long long>(frames - frames_written),
			static_cast<unsigned long long>(remaining_in_chunk));

		for (auto frame = 0; frame < frames_to_copy; ++frame) {
			auto chunk_frame = chunk.read_offset + frame;
			for (auto channel = 0; channel < m_parameters.channels; ++channel) {
				buffer[buffer_offset + frame * m_parameters.channels + channel] =
					chunk.data[chunk_frame * m_parameters.channels + channel];
			}
		}

		chunk.read_offset += frames_to_copy;
		frames_written += frames_to_copy;
		buffer_offset += frames_to_copy * m_parameters.channels;

		if (chunk.read_offset >= cAudioChunkSize) {
			m_queue.pop();
		}
	}
}