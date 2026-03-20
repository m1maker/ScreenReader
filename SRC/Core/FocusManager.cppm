module;
#include "Logger.h"

#include <memory>
#include <memory_resource>
#include <vector>
export module Core.FocusManager;
import Core.Environment;
import Core.Object;
import Proxies.Object;

export class CFocusManager final {
	std::pmr::unsynchronized_pool_resource m_pool;

	CObjectProxy m_objectInFocus;
	std::pmr::vector<CObjectProxy> m_contextChain;

	void UpdateContextChain() {
		LogCalled();
		m_contextChain.clear();
		if (!m_objectInFocus.IsValid())
			return;

		auto current = m_objectInFocus.GetParent();
		while (current && current->IsValid()) {
			m_contextChain.push_back(*current);

			auto type = current->GetType().value_or(EObjectType::UNKNOWN);
			if (IsObjectParent(type)) {
				break;
			}
			current = current->GetParent();
		}
	}

	explicit CFocusManager() : m_contextChain(&m_pool) {}
	~CFocusManager() = default;

public:
	static auto& GetInstance() {
		static CFocusManager instance;
		return instance;
	}

	void SetFocus(auto&& obj) {
		if (!obj.IsValid() || m_objectInFocus == obj)
			return;

		m_objectInFocus = obj;
		UpdateContextChain();
	}

	[[nodiscard]] auto GetContext() const { return m_contextChain; }

	[[nodiscard]] auto GetFocus() const { return m_objectInFocus; }
};
