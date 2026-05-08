module;
#include <array>
#include <cstdint>
#include <memory_resource>
#include <vector>
export module Core.OutputManager;
import Core.Event;
import Core.Logger;
import Core.Object;
import Core.Outputs;
import Core.Singleton;
import Proxies.Object;
import Proxies.Output;

export class OutputManager final : TModule<"OutputManager">, public TSingleton<OutputManager> {
	using OutputProxyMethodEvent = void (COutputProxy::*)(CObjectEvent);
	using OutputProxyMethodVoid = void (COutputProxy::*)(void);

	bool m_isWhereAmIOperation{false};

	std::pmr::vector<CObjectProxy> m_contextChain;

	static constexpr size_t cOutputCount = 1;
	std::array<COutputProxy, cOutputCount> m_outputs;

	template <OutputProxyMethodEvent Method> inline void WithAll(CObjectEvent event) {
		for (auto&& output : m_outputs) {
			(output.*Method)(event);
		}
	}
	template <OutputProxyMethodVoid Method> inline void WithAll(void) {
		for (auto&& output : m_outputs) {
			(output.*Method)();
		}
	}

public:
	explicit OutputManager() { m_outputs[0] = COutputProxy(COutputSpeech()); }

	inline void UnknownArea() {}

	auto WhereAmI() -> bool;

	inline void Stop() { WithAll<&COutputProxy::Stop>(); }

	inline void Output(CObjectEvent event) { WithAll<&COutputProxy::Output>(event); }
};
