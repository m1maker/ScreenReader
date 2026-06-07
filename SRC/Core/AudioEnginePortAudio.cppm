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
#include <portaudio.h>
export module Core.AudioEngine;
import Core.Audio;

export class CAudioEnginePortAudio final {
	PaStream* m_handle{nullptr};

	std::mutex m_callbackMutex;

	static int PaCallback([[maybe_unused]] const void* pInput,
		void* pOutput,
		unsigned long int frames_per_buffer,
		[[maybe_unused]] const PaStreamCallbackTimeInfo* pTimeInfo,
		[[maybe_unused]] PaStreamCallbackFlags status_flags,
		void* pUserData);

	[[nodiscard]] auto AudioCallback(signed short int* buffer, unsigned long long int frames) -> int;

	void Uninitialize();

public:
	~CAudioEnginePortAudio();

	[[nodiscard]] auto Initialize(SAudioParameters parameters) -> AudioEngineResult<>;
};

export using BuiltInAudioEngine = CAudioEnginePortAudio;
