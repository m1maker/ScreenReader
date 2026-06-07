/*
 * This file is part of the Screen Reader project.
 *
 * Copyright (C) 2025-2026  Georgiy Bondarenko (M_maker / m1maker) <georgijbondarenko248@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

module;
#include <array>
#include <atomic>
#include <cctype>
#include <cstdint>
#include <expected>
#include <mutex>
#include <queue>
#include <utility>
#include <variant>
#include <vector>
export module Core.AudioSystem;
import Core.Audio;
import Core.AudioEngine;
import Core.Logger;
import Core.Singleton;

export using AudioEngineVariant = std::variant<std::monostate, BuiltInAudioEngine /*, CAudioEngineRuntime*/>;

constexpr size_t cAudioChunkSize = 256;
constexpr size_t cAudioSystemMaxChannels = 255;

using AudioDataChunk = std::array<AudioSample, cAudioChunkSize>;

struct SAudioChunk final {
	unsigned char channel{0};
	AudioDataChunk data;
	size_t read_offset{0};
};

using AudioChunkArray = std::array<SAudioChunk, cAudioSystemMaxChannels>;
using AudioChunkSamplesArray = std::array<size_t, cAudioSystemMaxChannels>;

using AudioDataQueue = std::queue<SAudioChunk>;

export using AudioDataVector = std::vector<AudioSample>;

using AudioChannelAtomicMask = std::array<std::atomic<unsigned char>, cAudioSystemMaxChannels>;

export class AudioSystem final : TModule<"AudioSystem">, public TSingleton<AudioSystem> {
	AudioEngineVariant m_variant;
	AudioDataQueue m_queue;
	std::mutex m_mutex;
	SAudioParameters m_parameters;

	AudioChannelAtomicMask m_channelsShouldStop{};
	AudioChunkArray m_partialChunks;
	AudioChunkSamplesArray m_partialChunkSamples{};

	int m_bytesPerSample{0};
	int m_bytesPerFrame{0};

	template <typename Result = void> auto WithEngine(auto&& func) -> AudioEngineResult<Result> {
		return std::visit(
			[&](auto&& eng) -> AudioEngineResult<Result> {
				using T = std::decay_t<decltype(eng)>;
				if constexpr (!std::is_same_v<T, std::monostate>) {
					return func(eng);
				}
				else
					return std::unexpected(EAudioEngineError::DEFUNCT);
			},
			m_variant);
	}

	[[nodiscard]] inline auto Initialize(SAudioParameters parameters) -> AudioEngineResult<> {
		return WithEngine<>([parameters](auto&& engine) { return engine.Initialize(parameters); });
	}

public:
	explicit AudioSystem() { m_variant.emplace<BuiltInAudioEngine>(); }

	~AudioSystem() { Stop(); }

	void Start();
	void Stop(void);
	void Stop(unsigned char channel);

	void PushData(unsigned char channel, const AudioDataVector& data);

	void Read(signed short int* buffer, unsigned long long int frames);
};
