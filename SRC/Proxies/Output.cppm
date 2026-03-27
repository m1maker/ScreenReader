module;
#include <variant>
export module Proxies.Output;
import Core.Outputs;
import Proxies.Object;

template <typename Variant> class TUnknownProxy {
	Variant m_variant;

protected:
	TUnknownProxy() = default;
	explicit TUnknownProxy(Variant&& variant) : m_variant(std::move(variant)) {}

public:
	void With(auto&& func) /*final*/ {
		std::visit(
			[&](auto&& output) -> void {
				using T = std::decay_t<decltype(output)>;
				if constexpr (!std::is_same_v<T, std::monostate>) {
					func(output);
				}
			},
			m_variant);
	}
};

export class COutputProxy final : public TUnknownProxy<OutputVariant> {
public:
	COutputProxy() = default;
	explicit COutputProxy(OutputVariant&& variant) : TUnknownProxy(std::move(variant)) {}

	void Stop() {
		With([](auto&& out) { out.Stop(); });
	}

	void FocusChange(CObjectProxy obj) {
		With([obj](auto&& out) { out.FocusChange(obj); });
	}
	void StateChange(CObjectProxy obj) {
		With([obj](auto&& out) { out.StateChange(obj); });
	}
	void ValueChange(CObjectProxy obj) {
		With([obj](auto&& out) { out.ValueChange(obj); });
	}
	void CursorMove(CObjectProxy obj) {
		With([obj](auto&& out) { out.CursorMove(obj); });
	}
};
