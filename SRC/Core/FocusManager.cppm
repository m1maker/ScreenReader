module;

#include <memory>
#include <memory_resource>
#include <vector>
export module Core.FocusManager;
import Core.Environment;
import Core.Logger;
import Core.Object;
import Core.ObjectMeta;
import Core.ObjectNavigator;
import Core.Rotor;
import Core.Singleton;
import Proxies.Object;

export class FocusManager final : TModule<"FocusManager">, public TSingleton<FocusManager> {
	std::pmr::unsynchronized_pool_resource m_pool;

	CObjectProxy m_objectInFocus;
	std::pmr::vector<CObjectProxy> m_contextChain;

	void UpdateContextChain() {
		m_contextChain.clear();
		if (!m_objectInFocus.IsValid())
			return;

		auto current = m_objectInFocus.GetParent();
		while (current && current->IsValid()) {
			m_contextChain.insert(m_contextChain.begin(), *current);

			auto type = current->GetType().value_or(EObjectType::UNKNOWN);
			if (IsObjectInGroup(type, EObjectGroup::PARENT)) {
				break;
			}
			current = current->GetParent();
		}
	}

public:
	explicit FocusManager() : m_contextChain(&m_pool) {}
	~FocusManager() = default;

	void SetFocus(auto&& obj) {
		if (!obj.IsValid() || m_objectInFocus == obj)
			return;

		m_objectInFocus = obj;
		UpdateContextChain();
		Rotor::GetInstance().SetContext(obj);
		ObjectNavigator::GetInstance().SetFocus(obj);
	}

	[[nodiscard]] auto GetContext() const { return m_contextChain; }

	[[nodiscard]] auto GetFocus() const { return m_objectInFocus; }
};
