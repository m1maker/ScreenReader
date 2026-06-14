/*
 * This file is part of the Screen Reader project.
 *
 * Copyright (C) 2025-2026  Georgiy Bondarenko (M_maker / m1maker) <georgijbondarenko248@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

module;
#include <expected>
#include <optional>
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
	template <typename Result = void> auto With(this auto&& self, auto&& func) /*final*/ -> ObjectResult<Result> {
		return std::visit(
			[&](auto&& obj) -> ObjectResult<Result> {
				using T = std::decay_t<decltype(obj)>;
				if constexpr (!std::is_same_v<T, std::monostate>) {
					return func(obj);
				}
				else
					return std::unexpected(EObjectError::DEFUNCT);
			},
			self.m_variant);
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
	mutable std::optional<EObjectType> m_cachedType;
	mutable std::optional<ObjectStateMask> m_cachedStates;
	mutable std::optional<ObjectCapabilityMask> m_cachedCapabilities;
	mutable std::optional<int> m_cachedIndex;
	mutable std::optional<std::string_view> m_cachedApplicationName, m_cachedName, m_cachedDescription;

public:
	CObjectProxy() = default;
	explicit CObjectProxy(ObjectVariant object) : TUnknownProxy(object) {}
	~CObjectProxy() = default;

	[[nodiscard]] inline auto GetType() const -> ObjectResult<EObjectType> {
		if (m_cachedType)
			return *m_cachedType;
		auto result = With<EObjectType>([](auto&& obj) { return obj.GetType(); });
		if (result)
			m_cachedType = *result;
		return result;
	}
	[[nodiscard]] inline auto GetState() const -> ObjectResult<ObjectStateMask> {
		if (m_cachedStates)
			return *m_cachedStates;
		auto result = With<ObjectStateMask>([](auto&& obj) { return obj.GetState(); });
		if (result)
			m_cachedStates = *result;
		return result;
	}
	[[nodiscard]] inline auto GetCapabilities() const -> ObjectResult<ObjectCapabilityMask> {
		if (m_cachedCapabilities)
			return *m_cachedCapabilities;
		auto result = With<ObjectCapabilityMask>([](auto&& obj) { return obj.GetCapabilities(); });
		if (result)
			m_cachedCapabilities = *result;
		return result;
	}

	[[nodiscard]] inline auto GetParent() const -> ObjectResult<CObjectProxy> {
		ObjectResult<ObjectVariant> variant =
			With<ObjectVariant>([](auto&& obj) -> ObjectResult<ObjectVariant> { return obj.GetParent(); });
		if (variant) {
			return CObjectProxy(variant.value());
		}
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
		if (m_cachedIndex)
			return *m_cachedIndex;
		auto result = With<int>([](auto&& obj) { return obj.GetIndex(); });
		if (result)
			m_cachedIndex = *result;
		return result;
	}

	[[nodiscard]] inline auto GetBounds() const -> ObjectResult<SRect> {
		return With<SRect>([](auto&& obj) { return obj.GetBounds(); });
	}

	[[nodiscard]] inline auto GetApplicationName() const -> ObjectResult<std::string_view> {
		if (m_cachedApplicationName)
			return *m_cachedApplicationName;
		auto result = With<std::string_view>([](auto&& obj) { return obj.GetApplicationName(); });
		if (result)
			m_cachedApplicationName = *result;
		return result;
	}
	[[nodiscard]] inline auto GetName() const -> ObjectResult<std::string_view> {
		if (m_cachedName)
			return *m_cachedName;
		auto result = With<std::string_view>([](auto&& obj) { return obj.GetName(); });
		if (result)
			m_cachedName = *result;
		return result;
	}
	[[nodiscard]] inline auto GetDescription() const -> ObjectResult<std::string_view> {
		if (m_cachedDescription)
			return *m_cachedDescription;
		auto result = With<std::string_view>([](auto&& obj) { return obj.GetDescription(); });
		if (result)
			m_cachedDescription = *result;
		return result;
	}
};

export class CTextProviderProxy final : public TUnknownProxy<TextProviderVariant> {
public:
	explicit CTextProviderProxy(TextProviderVariant provider) : TUnknownProxy(provider) {}
	~CTextProviderProxy() = default;

	[[nodiscard]] inline auto GetCursor() const -> ObjectResult<int> {
		return With<int>([](auto&& obj) { return obj.GetCursor(); });
	}
	[[nodiscard]] inline auto GetText(int cursor, ETextGranularity granularity) const -> ObjectResult<STextRange> {
		return With<STextRange>([cursor, granularity](auto&& obj) { return obj.GetText(cursor, granularity); });
	}
	[[nodiscard]] inline auto GetSelected() const -> ObjectResult<STextRange> {
		return With<STextRange>([](auto&& obj) { return obj.GetSelectedText(); });
	}
};

export class CSelectionProviderProxy final : public TUnknownProxy<SelectionProviderVariant> {
public:
	explicit CSelectionProviderProxy(SelectionProviderVariant provider) : TUnknownProxy(provider) {}
	~CSelectionProviderProxy() = default;

	[[nodiscard]] inline auto GetChildrenCount() const -> ObjectResult<int> {
		return With<int>([](auto&& obj) { return obj.GetSelectedChildrenCount(); });
	}

	[[nodiscard]] inline auto GetChildAt(int index) const -> ObjectResult<CObjectProxy> {
		ObjectResult<ObjectVariant> variant = With<ObjectVariant>(
			[index](auto&& obj) -> ObjectResult<ObjectVariant> { return obj.GetSelectedChildAt(index); });
		if (variant)
			return ObjectResult<CObjectProxy>(variant.value());
		return std::unexpected(variant.error());
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

export class CActionProviderProxy final : public TUnknownProxy<ActionProviderVariant> {
public:
	explicit CActionProviderProxy(ActionProviderVariant provider) : TUnknownProxy(provider) {}
	~CActionProviderProxy() = default;

	[[nodiscard]] inline auto GetCount() const -> ObjectResult<int> {
		return With<int>([](auto&& obj) { return obj.GetActionCount(); });
	}

	[[nodiscard]] inline auto GetType(int number) const -> ObjectResult<EObjectAction> {
		return With<EObjectAction>([number](auto&& obj) { return obj.GetActionType(number); });
	}
	[[nodiscard]] inline auto GetName(int number) const -> ObjectResult<std::string_view> {
		return With<std::string_view>([number](auto&& obj) { return obj.GetActionName(number); });
	}
	[[nodiscard]] inline auto Do(int number) -> ObjectResult<> {
		return With<>([number](auto&& obj) { return obj.DoAction(number); });
	}
};
