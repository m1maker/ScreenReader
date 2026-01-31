#pragma once
#include "Object.h"
#include "Singleton.h"
#include <memory>
#include <vector>

class CFocusManager final {
	DeclareSingleton(CFocusManager);

	std::shared_ptr<IObject> m_objectInFocus;
	std::vector<std::shared_ptr<IObject>> m_contextChain;

	void UpdateContextChain() {
		m_contextChain.clear();
		if (!m_objectInFocus) return;

		auto current = m_objectInFocus->GetParent().lock();
		while (current) {
			m_contextChain.push_back(current);

			auto type = current->GetType();
			if (IObject::IsValidParent(type)) {
				break;
			}
			current = current->GetParent().lock();
		}
	}

	explicit CFocusManager() = default;
	~CFocusManager() = default;
public:

	void SetFocus(std::shared_ptr<IObject> obj) {
		if (!obj || m_objectInFocus == obj) return;

		m_objectInFocus = obj;
		UpdateContextChain();
	}

	[[nodiscard]] auto GetContext() const -> const std::vector<std::shared_ptr<IObject>>& {
		return m_contextChain;
	}

	[[nodiscard]] auto GetFocus() const -> std::shared_ptr<IObject> {
		return m_objectInFocus;
	}
};

#define g_focusManager CSingleton<CFocusManager>::GetInstance()
