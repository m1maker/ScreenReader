module;
#include <algorithm>
#include <array>
#include <cstdint>
#include <memory_resource>
#include <string>
#include <variant>
#include <vector>
export module Core.OutputManager;
import Core.FocusManager;
import Core.Outputs;
import Proxies.Object;
import Proxies.Output;

export class OutputManager final {
	using OutputProxyMethodObject = void (COutputProxy::*)(CObjectProxy);
	using OutputProxyMethodVoid = void (COutputProxy::*)(void);

	bool m_isWhereAmIOperation{false};

	FocusManager& m_focusManager;
	std::pmr::vector<CObjectProxy> m_contextChain;

	static constexpr size_t cOutputCount = 1;
	std::array<COutputProxy, cOutputCount> m_outputs;

	template <OutputProxyMethodObject Method> inline void WithAll(CObjectProxy obj) {
		for (auto&& output : m_outputs) {
			(output.*Method)(obj);
		}
	}
	template <OutputProxyMethodVoid Method> inline void WithAll(void) {
		for (auto&& output : m_outputs) {
			(output.*Method)();
		}
	}

	OutputManager() : m_focusManager(FocusManager::GetInstance()) { m_outputs[0] = COutputProxy(COutputSpeech()); }

public:
	static auto& GetInstance() {
		static OutputManager instance;
		return instance;
	}

	inline void UnknownArea() {}

	/*
	This method tries to find parents who have not been announced or have changed, and pushes the announcements without
	"Stop()" in reverse order, except the last object.
	*/
	auto WhereAmI() -> bool {
		auto object = m_focusManager.GetFocus();
		if (!object.IsValid()) {
			UnknownArea();
			return true;
		}

		const auto chain = m_focusManager.GetContext();

		size_t diff_index{0};
		size_t min_size = std::min(chain.size(), m_contextChain.size());

		while (diff_index < min_size && chain[diff_index] == m_contextChain[diff_index]) {
			++diff_index;
		}

		if (diff_index == chain.size() && chain.size() == m_contextChain.size()) {
			return false;
		}

		std::string last_name{""};
		m_isWhereAmIOperation = true;
		for (size_t i = diff_index; i < chain.size(); ++i) {
			const auto& current_object = chain[i];

			auto it = std::ranges::find(m_contextChain, current_object);
			if (it != m_contextChain.end())
				continue;

			auto current_name = current_object.GetName().value_or("");

			if (current_name.empty() || current_name == last_name) {
				continue;
			}

			last_name = current_name;

			FocusChange(current_object);
		}
		m_isWhereAmIOperation = false;
		m_contextChain = chain;
		return true;
	}

	inline void Stop() { WithAll<&COutputProxy::Stop>(); }

	inline void FocusChange(CObjectProxy obj) { WithAll<&COutputProxy::FocusChange>(obj); }
	inline void StateChange(CObjectProxy obj) { WithAll<&COutputProxy::StateChange>(obj); }
	inline void ValueChange(CObjectProxy obj) { WithAll<&COutputProxy::ValueChange>(obj); }
	inline void CursorMove(CObjectProxy obj) { WithAll<&COutputProxy::CursorMove>(obj); }
};
