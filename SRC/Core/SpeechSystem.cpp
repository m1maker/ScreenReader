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
#include <mutex>
#include <stop_token>
#include <thread>
#include <variant>
module Core.SpeechSystem;
import Core.App;
import Core.AudioSystem;
import Core.Speech;

void SpeechSystem::ApplySpeechParameters(SpeechParameters parameters) {
	m_rate.store(parameters.rate);
	m_volume.store(parameters.volume);
	m_pitch.store(parameters.pitch);
	m_pitchRange.store(parameters.pitch_range);
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

			auto result = EngineSetParameter(ESpeechEngineParameter::RATE, m_rate.load());
			result = EngineSetParameter(ESpeechEngineParameter::PITCH, m_pitch.load());
			result = EngineSetParameter(ESpeechEngineParameter::VOLUME, m_volume.load());
			result = EngineSetParameter(ESpeechEngineParameter::PITCH_RANGE, m_pitchRange.load());
			if (!result) {
			}

			while (!message.command_queue.empty()) {
				auto command = std::move(message.command_queue.front());
				message.command_queue.pop();
				using enum EUtteranceCommandType;
				switch (command.type) {
				case TEXT: {
					auto text = std::visit(
						[](auto&& value) -> std::string {
							using T = std::decay_t<decltype(value)>;
							if constexpr (std::is_same_v<T, std::string>)
								return std::move(value);
							return "";
						},
						std::move(command.value));
					auto result = EngineSpeak(text);
					if (!result) {
					}
					break;
				}
				case PITCH: {
					auto pitch = std::visit(
						[](auto&& value) -> unsigned char {
							using T = std::decay_t<decltype(value)>;
							if constexpr (std::is_same_v<T, unsigned char>)
								return std::move(value);
							return +EUtterancePitchValue::DEFAULT;
						},
						std::move(command.value));

					if (pitch == 0)
						continue;
					unsigned char current_value{0};
					auto result = EngineGetParameter(ESpeechEngineParameter::PITCH, current_value);
					unsigned char absolute_value{+EUtterancePitchValue::DEFAULT};
					if (result) {
						signed char offset = cSpeechEngineNormalValue - current_value;
						absolute_value =
							std::clamp(static_cast<unsigned char>(command.relative ? (pitch + offset) : (pitch)),
								cSpeechEngineMinValue,
								cSpeechEngineMaxValue);
					}

					result = EngineSetParameter(ESpeechEngineParameter::PITCH, absolute_value);
					if (!result) {
					}
					break;
				}

				case RATE: {
					auto rate = std::visit(
						[](auto&& value) -> unsigned char {
							using T = std::decay_t<decltype(value)>;
							if constexpr (std::is_same_v<T, unsigned char>)
								return std::move(value);
							return +EUtteranceRateValue::DEFAULT;
						},
						std::move(command.value));
					if (rate == 0)
						continue;

					unsigned char current_value{0};
					auto result = EngineGetParameter(ESpeechEngineParameter::RATE, current_value);
					unsigned char absolute_value{+EUtteranceRateValue::DEFAULT};
					if (result) {
						signed char offset = cSpeechEngineNormalValue - current_value;
						absolute_value =
							std::clamp(static_cast<unsigned char>(command.relative ? (rate + offset) : (rate)),
								cSpeechEngineMinValue,
								cSpeechEngineMaxValue);
					}

					result = EngineSetParameter(ESpeechEngineParameter::RATE, absolute_value);
					if (!result) {
					}
					break;
				}

				default:
					break;
				}
			}
		}
	});
}

void SpeechSystem::Stop() {
	m_thread.request_stop();
	m_cv.notify_all();
}

void SpeechSystem::Speak(UtteranceCommandQueue&& utterance_commands, bool interrupt) {
	ApplySpeechParameters(ScreenReaderApp::GetInstance().GetSettings().speech);

	SSpeechMessage queued_message{
		.command_queue = std::move(utterance_commands),
		.interrupt = interrupt,
		.ssml = m_ssml,
	};

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