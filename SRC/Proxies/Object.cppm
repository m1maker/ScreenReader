module;
#include <expected>
#include <string_view>
#include <variant>
export module Proxies.Object;
import Core.Environment;
import Core.Object;
import Core.Rect;
import Core.Text;

template <typename Variant> class TUnknownProxy {
	Variant m_variant;

protected:
	TUnknownProxy() = default;
	explicit TUnknownProxy(Variant variant) : m_variant(variant) {}

public:
	template <typename Result = void> auto With(auto&& func) const /*final*/ -> ObjectResult<Result> {
		return std::visit(
			[&](auto&& obj) -> ObjectResult<Result> {
				using T = std::decay_t<decltype(obj)>;
				if constexpr (!std::is_same_v<T, std::monostate>) {
					return func(obj);
				}
				else
					return std::unexpected(EObjectError::DEFUNCT);
			},
			m_variant);
	}

	template <typename Provider> [[nodiscard]] auto GetAs() const -> Provider { return Provider(m_variant); }

	bool IsValid() const {
		auto valid = std::visit(
			[&](auto&& obj) -> bool {
				using T = std::decay_t<decltype(obj)>;
				if constexpr (!std::is_same_v<T, std::monostate>) {
					if constexpr (std::is_same_v<Variant, ObjectVariant>)
						return obj.IsValid();
					return true;
				}
				else
					return false;
			},
			m_variant);
		return valid;
	}
	[[nodiscard]] auto operator==(const TUnknownProxy& other) const -> bool { return m_variant == other.m_variant; }
};

export class CObjectProxy final : public TUnknownProxy<ObjectVariant> {
public:
	CObjectProxy() = default;
	explicit CObjectProxy(ObjectVariant object) : TUnknownProxy(object) {}
	~CObjectProxy() = default;

	[[nodiscard]] inline auto GetType() const -> ObjectResult<EObjectType> {
		return With<EObjectType>([](auto&& obj) { return obj.GetType(); });
	}
	[[nodiscard]] inline auto GetState() const -> ObjectResult<ObjectStates> {
		return With<ObjectStates>([](auto&& obj) { return obj.GetState(); });
	}

	[[nodiscard]] inline auto GetParent() const -> ObjectResult<CObjectProxy> {
		ObjectResult<ObjectVariant> variant =
			With<ObjectVariant>([](auto&& obj) -> ObjectResult<ObjectVariant> { return obj.GetParent(); });
		if (variant)
			return ObjectResult<CObjectProxy>(variant.value());
		return std::unexpected(variant.error());
	}
	[[nodiscard]] inline auto GetChildrenCount() const -> ObjectResult<int> {
		return With<int>([](auto&& obj) { return obj.GetChildrenCount(); });
	}

	[[nodiscard]] inline auto GetChildAt(int index) const -> ObjectResult<CObjectProxy> {
		ObjectResult<ObjectVariant> variant =
			With<ObjectVariant>([index](auto&& obj) -> ObjectResult<ObjectVariant> { return obj.GetChildAt(index); });
		if (variant)
			return ObjectResult<CObjectProxy>(variant.value());
		return std::unexpected(variant.error());
	}
	[[nodiscard]] inline auto GetIndex() const -> ObjectResult<int> {
		return With<int>([](auto&& obj) { return obj.GetIndex(); });
	}

	[[nodiscard]] inline auto GetBounds() const -> ObjectResult<SRect> {
		return With<SRect>([](auto&& obj) { return obj.GetBounds(); });
	}

	[[nodiscard]] inline auto GetApplicationName() const -> ObjectResult<std::string> {
		return With<std::string>([](auto&& obj) { return obj.GetApplicationName(); });
	}
	[[nodiscard]] inline auto GetName() const -> ObjectResult<std::string> {
		return With<std::string>([](auto&& obj) { return obj.GetName(); });
	}
	[[nodiscard]] inline auto GetDescription() const -> ObjectResult<std::string> {
		return With<std::string>([](auto&& obj) { return obj.GetDescription(); });
	}
};

export class CTextProviderProxy final : public TUnknownProxy<TextProviderVariant> {
public:
	explicit CTextProviderProxy(TextProviderVariant provider) : TUnknownProxy(provider) {}
	~CTextProviderProxy() = default;

	[[nodiscard]] inline auto GetCursor() const -> ObjectResult<int> {
		return With<int>([](auto&& obj) { return obj.GetCursor(); });
	}
	[[nodiscard]] inline auto GetText(int cursor, ETextGranularity granularity) const
		-> ObjectResult<STextRange<std::string>> {
		return With<STextRange<std::string>>(
			[cursor, granularity](auto&& obj) { return obj.GetText(cursor, granularity); });
	}
};

export class CValueProviderProxy final : public TUnknownProxy<ValueProviderVariant> {
public:
	explicit CValueProviderProxy(ValueProviderVariant provider) : TUnknownProxy(provider) {}
	~CValueProviderProxy() = default;

	[[nodiscard]] inline auto GetMin() const -> ObjectResult<double> {
		return With<double>([](auto&& obj) { return obj.GetMinValue(); });
	}
	[[nodiscard]] inline auto GetMax() const -> ObjectResult<double> {
		return With<double>([](auto&& obj) { return obj.GetMaxValue(); });
	}
	[[nodiscard]] inline auto GetCurrent() const -> ObjectResult<double> {
		return With<double>([](auto&& obj) { return obj.GetCurrentValue(); });
	}
};
