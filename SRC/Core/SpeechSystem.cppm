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
#include <atomic>
#include <cctype>
#include <condition_variable>
#include <cstdint>
#include <expected>
#include <memory_resource>
#include <mutex>
#include <queue>
#include <string_view>
#include <thread>
#include <utility>
#include <variant>
export module Core.SpeechSystem;
import Core.BuiltInSpeechEngine;
import Core.Config;
import Core.Encoding;
import Core.Logger;
import Core.MessageBuilder;
import Core.Singleton;
import Core.Speech;

export using SpeechEngineVariant = std::variant<std::monostate, BuiltInSpeechEngine /*, CSpeechEngineRuntime*/>;

struct SSpeechMessage final {
	std::pmr::string message;
	bool interrupt{false}, ssml{false};
	unsigned char rate, volume, pitch, pitch_range;
};

using SpeechMessageQueue = std::queue<SSpeechMessage>;

export class SpeechSystem final : TModule<"SpeechSystem">, public TSingleton<SpeechSystem> {
	unsigned char m_rate, m_volume, m_pitch, m_pitchRange;
	bool m_ssml{false};
	std::pmr::memory_resource* m_pool{nullptr};
	SpeechEngineVariant m_variant;
	SpeechMessageQueue m_queue;
	std::mutex m_mutex;
	std::condition_variable m_cv;
	std::jthread m_thread;

	std::atomic_flag m_shouldAbort{ATOMIC_FLAG_INIT}; // Speech engine with the callback function must request it.

	void ApplySpeechParameters(SpeechParameters parameters);

	template <typename Result = void> auto WithEngine(this auto&& self, auto&& func) -> SpeechEngineResult<Result> {
		return std::visit(
			[&](auto&& eng) -> SpeechEngineResult<Result> {
				using T = std::decay_t<decltype(eng)>;
				if constexpr (!std::is_same_v<T, std::monostate>) {
					return func(eng);
				}
				else
					return std::unexpected(ESpeechEngineError::DEFUNCT);
			},
			self.m_variant);
	}

	inline auto EngineGetInfo() const -> SpeechEngineResult<SSpeechEngineInfo> {
		return WithEngine<SSpeechEngineInfo>([](auto&& engine) { return engine.GetInfo(); });
	}

	inline auto EngineTest() const -> SpeechEngineResult<> {
		return WithEngine<>([](auto&& engine) { return engine.Test(); });
	}

	inline auto EngineSpeak(std::string_view message) -> SpeechEngineResult<SpeechMessage> {
		return WithEngine<SpeechMessage>([message](auto&& engine) { return engine.Speak(message); });
	}

	void EngineStop() {
		WithEngine<>([](auto&& engine) {
			engine.Stop();
			return SpeechEngineResult<>();
		});
	}

	inline auto EngineSetParameter(ESpeechEngineParameter parameter, auto&& value) -> SpeechEngineResult<> {
		return WithEngine<>([parameter, value](auto&& engine) { return engine.SetParameter(parameter, value); });
	}

	template <typename Value>
	[[nodiscard]] inline auto EngineGetParameter(ESpeechEngineParameter parameter) const -> SpeechEngineResult<Value> {
		return WithEngine<Value>([parameter](auto&& engine) { return engine.GetParameter(parameter); });
	}

	[[nodiscard]] inline auto EngineGetVoiceInfo(unsigned long long int index) const -> SpeechEngineResult<SVoiceInfo> {
		return WithEngine<SVoiceInfo>([index](auto&& engine) { return engine.GetVoiceInfo(index); });
	}

public:
	explicit SpeechSystem() : m_pool(MessageBuilder::GetInstance().GetPool()) {
		m_variant.emplace<BuiltInSpeechEngine>();
	}

	~SpeechSystem() { Stop(); }

	void Start();
	void Stop();

	void Speak(std::string_view message, bool interrupt = false);
	void Interrupt();

	[[nodiscard]] auto ShouldAbort() const noexcept -> bool { return m_shouldAbort.test(std::memory_order_release); }
};
