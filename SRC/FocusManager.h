#pragma once
#include "Singleton.h"
#include <memory>

class CFocusManager final {
	DeclareSingleton(CFocusManager);

	std::shared_ptr<class IObject> m_objectInFocus;
	friend class CEventHandler;
	explicit CFocusManager() = default;
	~CFocusManager() = default;
public:

	[[nodiscard]] auto GetFocus() const -> std::shared_ptr<class IObject> {
		return m_objectInFocus;
	}
};

#define g_focusManager CSingleton<CFocusManager>::GetInstance()
