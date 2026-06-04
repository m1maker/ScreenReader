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
#include <algorithm>
#include <cstdint>
#include <string_view>
module Core.OutputManager;
import Core.FocusManager;
import Proxies.Output;

/*
This method tries to find parents who have not been announced or have changed, and pushes the announcements without
"Stop()" in reverse order, except the last object.
*/
auto OutputManager::WhereAmI() -> bool {
	auto& focus_manager = FocusManager::GetInstance();
	auto object = focus_manager.GetFocus();
	if (!object.IsValid()) {
		UnknownArea();
		return true;
	}

	const auto chain = focus_manager.GetContext();

	size_t diff_index{0};
	size_t min_size = std::min(chain.size(), m_contextChain.size());

	while (diff_index < min_size && chain[diff_index] == m_contextChain[diff_index]) {
		++diff_index;
	}

	if (diff_index == chain.size() && chain.size() == m_contextChain.size()) {
		return false;
	}

	std::string_view last_name{};
	m_isWhereAmIOperation = true;
	for (size_t i = diff_index; i < chain.size(); ++i) {
		const auto current_object = chain[i];

		auto it = std::ranges::find(m_contextChain, current_object);
		if (it != m_contextChain.end())
			continue;

		auto current_name = current_object.GetName().value_or("");

		if (current_name.empty() || current_name == last_name) {
			continue;
		}

		last_name = current_name;
		CObjectEvent event{.type = EObjectEventType::FOCUS_GAINED, .object = current_object};
		Output(event);
	}
	m_isWhereAmIOperation = false;
	m_contextChain = chain;
	return true;
}
