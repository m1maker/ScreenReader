module;
#include <expected>
#include <string_view>
#include <variant>
export module Core.ObjectAccessor;
import Core.Environment;
import Core.Rect;
import Traits.Object;

template <typename Variant> class CVariantAccessor {
	Variant m_variant;

protected:
	explicit CVariantAccessor(Variant variant) : m_variant(variant) {}
	~CVariantAccessor() = default;

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
};

export class CObjectAccessor final : public CVariantAccessor<ObjectVariant> {
public:
	explicit CObjectAccessor(ObjectVariant object) : CVariantAccessor(object) {}
	~CObjectAccessor() = default;

	[[nodiscard]] inline auto GetType() const -> ObjectResult<EObjectType> {
		return With<EObjectType>([](auto&& obj) { return obj.GetType(); });
	}
	[[nodiscard]] inline auto GetState() const -> ObjectResult<unsigned long long> {
		return With<unsigned long long>([](auto&& obj) { return obj.GetState(); });
	}

	[[nodiscard]] inline auto GetParent() const -> ObjectResult<CObjectAccessor> {
		ObjectResult<ObjectVariant> variant =
			With<ObjectVariant>([](auto&& obj) -> ObjectResult<ObjectVariant> { return obj.GetParent(); });
		if (variant)
			return ObjectResult<CObjectAccessor>(variant.value());
		return std::unexpected(variant.error());
	}
	[[nodiscard]] inline auto GetChildrenCount() const -> ObjectResult<int> {
		return With<int>([](auto&& obj) { return obj.GetChildrenCount(); });
	}

	[[nodiscard]] inline auto GetChildAt(int index) const -> ObjectResult<CObjectAccessor> {
		ObjectResult<ObjectVariant> variant =
			With<ObjectVariant>([index](auto&& obj) -> ObjectResult<ObjectVariant> { return obj.GetChildAt(index); });
		if (variant)
			return ObjectResult<CObjectAccessor>(variant.value());
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
