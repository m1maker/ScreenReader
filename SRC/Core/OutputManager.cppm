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
#include <cstdint>
#include <memory_resource>
#include <vector>
export module Core.OutputManager;
import Core.Event;
import Core.Logger;
import Core.Object;
import Core.Outputs;
import Core.Singleton;
import Proxies.Object;
import Proxies.Output;

export class OutputManager final : TModule<"OutputManager">, public TSingleton<OutputManager> {
	using OutputProxyMethodEvent = void (COutputProxy::*)(CObjectEvent);
	using OutputProxyMethodVoid = void (COutputProxy::*)(void);

	bool m_isWhereAmIOperation{false};

	std::pmr::vector<CObjectProxy> m_contextChain;

	static constexpr size_t cOutputCount = 1;
	std::array<COutputProxy, cOutputCount> m_outputs;

	template <OutputProxyMethodEvent Method> inline void WithAll(CObjectEvent event) {
		for (auto&& output : m_outputs) {
			(output.*Method)(event);
		}
	}
	template <OutputProxyMethodVoid Method> inline void WithAll(void) {
		for (auto&& output : m_outputs) {
			(output.*Method)();
		}
	}

public:
	explicit OutputManager() { m_outputs[0] = COutputProxy(COutputSpeech()); }

	inline void UnknownArea() {}

	auto WhereAmI() -> bool;

	inline void Stop() { WithAll<&COutputProxy::Stop>(); }

	inline void Output(CObjectEvent event) { WithAll<&COutputProxy::Output>(event); }
};
