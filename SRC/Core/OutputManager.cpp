module;
#include <algorithm>
#include <cstdint>
#include <string>
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

	std::string last_name{};
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
