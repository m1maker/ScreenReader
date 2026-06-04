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
#include <variant>
export module Proxies.Output;
import Core.Event;
import Core.Outputs;
import Proxies.Object;

template <typename Variant> class TUnknownProxy {
	Variant m_variant;

protected:
	TUnknownProxy() = default;
	explicit TUnknownProxy(Variant&& variant) : m_variant(std::move(variant)) {}

public:
	void With(auto&& func) /*final*/ {
		std::visit(
			[&](auto&& output) -> void {
				using T = std::decay_t<decltype(output)>;
				if constexpr (!std::is_same_v<T, std::monostate>) {
					func(output);
				}
			},
			m_variant);
	}
};

export class COutputProxy final : public TUnknownProxy<OutputVariant> {
public:
	COutputProxy() = default;
	explicit COutputProxy(OutputVariant&& variant) : TUnknownProxy(std::move(variant)) {}

	void Stop() {
		With([](auto&& out) { out.Stop(); });
	}

	void Output(CObjectEvent event) {
		With([event](auto&& out) { out.Output(event); });
	}
};
