module;

#include <memory>
#include <memory_resource>
#include <vector>
export module Core.FocusManager;
import Core.Environment;
import Core.Logger;
import Core.Object;
import Proxies.Object;

export class FocusManager final : TModule<"FocusManager"> {
	std::pmr::unsynchronized_pool_resource m_pool;

	CObjectProxy m_objectInFocus;
	std::pmr::vector<CObjectProxy> m_contextChain;

	void UpdateContextChain() {
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

	explicit FocusManager() : m_contextChain(&m_pool) {}
	~FocusManager() = default;

public:
	static auto& GetInstance() {
		static FocusManager instance;
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
