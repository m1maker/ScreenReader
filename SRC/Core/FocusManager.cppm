module;
#include "Logger.h"

#include <memory>
#include <memory_resource>
#include <vector>
export module Core.FocusManager;
import Core.Environment;
import Traits.Object;

export class CFocusManager final {
	DeclareSingleton(CFocusManager);

	std::pmr::unsynchronized_pool_resource m_pool;

	CObject m_objectInFocus;
	std::pmr::vector<CObject> m_contextChain;

	void UpdateContextChain() {
		LogCalled();
		m_contextChain.clear();
		if (!m_objectInFocus.IsValid())
			return;

		auto current = m_objectInFocus.GetParent().value_or(CObject());
		while (current.IsValid()) {
			m_contextChain.push_back(current);

			auto type = current.GetType().value_or(EObjectType::UNKNOWN);
			if (IsObjectParent(type)) {
				break;
			}
			current = current.GetParent().value_or(CObject());
		}
	}

	explicit CFocusManager() : m_contextChain(&m_pool) {}
	~CFocusManager() = default;

public:
	static auto& GetInstance() {
		static CFocusManager instance;
		return instance;
	}

	void SetFocus(CObject obj) {
		if (!obj.IsValid() || m_objectInFocus == obj)
			return;

		m_objectInFocus = obj;
		UpdateContextChain();
	}

	[[nodiscard]] auto GetContext() const -> const std::pmr::vector<CObject> { return m_contextChain; }

	[[nodiscard]] auto GetFocus() const -> CObject { return m_objectInFocus; }
};
