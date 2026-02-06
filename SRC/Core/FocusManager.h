#pragma once
#include "Logger.h"
#include <Interfaces/Object.h>
#include "Singleton.h"
#include <memory>
#include <memory_resource>
#include <vector>

class CFocusManager final {
	DeclareSingleton(CFocusManager);

	std::pmr::unsynchronized_pool_resource m_pool;

	std::shared_ptr<IObject> m_objectInFocus;
	std::pmr::vector<std::shared_ptr<IObject>> m_contextChain;

	void UpdateContextChain() {
		LogCalled();
		m_contextChain.clear();
		if (!m_objectInFocus) return;

		auto current = m_objectInFocus->GetParent();
		if (!current) return;
		auto current_locked = current->lock();
		while (current_locked) {
			m_contextChain.push_back(current_locked);

			auto type = current_locked->GetType().value_or(IObject::UNKNOWN);
			if (IObject::IsValidParent(type)) {
				break;
			}
			auto next_current = current_locked->GetParent();
			if (!next_current) break;
			current_locked = next_current->lock();
		}
	}

	explicit CFocusManager() : m_contextChain(&m_pool) {}
	~CFocusManager() = default;
public:

	void SetFocus(std::shared_ptr<IObject> obj) {
		if (!obj || m_objectInFocus == obj) return;

		m_objectInFocus = obj;
		UpdateContextChain();
	}

	[[nodiscard]] auto GetContext() const -> const std::pmr::vector<std::shared_ptr<IObject>> {
		return m_contextChain;
	}

	[[nodiscard]] auto GetFocus() const -> std::shared_ptr<IObject> {
		return m_objectInFocus;
	}
};

#define g_focusManager CSingleton<CFocusManager>::GetInstance()
