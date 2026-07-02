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
import Core.ObjectCache;
import Core.Rect;
import Core.Text;
import Traits.NonAtomicRefCountedObject;

class UnknownProxy : /*protected*/ public TNonAtomicRefCountedObject<UnknownProxy, SCachedObjectData> {
protected:
	UnknownProxy() = default;
	explicit UnknownProxy(void* memory) : TNonAtomicRefCountedObject(memory) {}

	[[nodiscard]] inline auto GetVariant() const noexcept -> ObjectVariant* { return &GetData()->variant; }
	[[nodiscard]] inline auto* GetCachedProperties() const noexcept /*->unknown*/ { return &GetData()->properties; }

public:
	// TNonAtomicRefCountedObject
	void do_OnDestroy() noexcept {
		static_cast<void>(With<>([](auto&& obj) {
			obj.OnDestroy();
			return ObjectResult<>();
		}));
	}
	void InvalidateCacheByEvent(EObjectEventType event) const noexcept {
		if (!GetCachedProperties()) [[unlikely]]
			return;
		using enum EObjectEventType;
		switch (event) {
		case STATE_CHANGED:
		case SELECTION_CHANGED:
			GetCachedProperties()->states.reset();
			break;
		case LAYOUT_UPDATED:
			GetCachedProperties()->index.reset();
			break;
		case NAME_CHANGED:
			GetCachedProperties()->name.reset();
			break;
		case DESCRIPTION_CHANGED:
			GetCachedProperties()->description.reset();
			break;
		case CURSOR_MOVED:
			GetCachedProperties()->cursor.reset();
			break;
		case VALUE_CHANGED:
			GetCachedProperties()->min_value.reset();
			GetCachedProperties()->max_value.reset();
			GetCachedProperties()->current_value.reset();
			break;
		default:
			break;
		}
	}

	template <typename Result = void> auto With(this auto&& self, auto&& func) /*final*/ -> ObjectResult<Result> {
		if (!self.GetVariant()) [[unlikely]]
			return std::unexpected(EObjectError::DEFUNCT);
		return std::visit(
			[&](auto&& obj) -> ObjectResult<Result> {
				using T = std::decay_t<decltype(obj)>;
				if constexpr (!std::is_same_v<T, std::monostate>) {
					return func(obj);
				}
				else
					return std::unexpected(EObjectError::DEFUNCT);
			},
			*self.GetVariant());
	}

	template <typename Provider> [[nodiscard]] auto GetAs() const -> Provider { return Provider(*this); }

	bool IsValid() const {
		auto valid = GetVariant() &&
			std::visit(
				[&](auto&& obj) -> bool {
					using T = std::decay_t<decltype(obj)>;
					if constexpr (!std::is_same_v<T, std::monostate>) {
						return obj.IsValid();
					}
					else
						return false;
				},
				*GetVariant());
		return valid;
	}
	[[nodiscard]] auto operator==(const UnknownProxy& other) const noexcept { return GetData() == other.GetData(); }
};

export class CObjectProxy final : public UnknownProxy {
public:
	CObjectProxy() = default;
	explicit CObjectProxy(void* memory) : UnknownProxy(memory) {}

	[[nodiscard]] inline auto GetType() const -> ObjectResult<EObjectType> {
		if (GetCachedProperties() && GetCachedProperties()->type)
			return *GetCachedProperties()->type;
		auto result = With<EObjectType>([](auto&& obj) { return obj.GetType(); });
		if (result && GetCachedProperties())
			GetCachedProperties()->type = *result;
		return result;
	}
	[[nodiscard]] inline auto GetState() const -> ObjectResult<ObjectStateMask> {
		if (GetCachedProperties() && GetCachedProperties()->states)
			return *GetCachedProperties()->states;
		auto result = With<ObjectStateMask>([](auto&& obj) { return obj.GetState(); });
		if (result && GetCachedProperties())
			GetCachedProperties()->states = *result;
		return result;
	}
	[[nodiscard]] inline auto GetCapabilities() const -> ObjectResult<ObjectCapabilityMask> {
		if (GetCachedProperties() && GetCachedProperties()->capabilities)
			return *GetCachedProperties()->capabilities;
		auto result = With<ObjectCapabilityMask>([](auto&& obj) { return obj.GetCapabilities(); });
		if (result && GetCachedProperties())
			GetCachedProperties()->capabilities = *result;
		return result;
	}

	[[nodiscard]] inline auto GetParent() const -> ObjectResult<CObjectProxy> {
		auto result = With<void*>([](auto&& obj) { return obj.GetParent(); });
		if (!result)
			return std::unexpected(result.error());
		return CObjectProxy(*result);
	}

	[[nodiscard]] inline auto GetChildrenCount() const -> ObjectResult<int> {
		return With<int>([](auto&& obj) { return obj.GetChildrenCount(); });
	}

	[[nodiscard]] inline auto GetChildAt(int index) const -> ObjectResult<CObjectProxy> {
		auto result = With<void*>([index](auto&& obj) { return obj.GetChildAt(index); });
		if (!result)
			return std::unexpected(result.error());
		return CObjectProxy(*result);
	}
	[[nodiscard]] inline auto GetIndex() const -> ObjectResult<int> {
		if (GetCachedProperties() && GetCachedProperties()->index)
			return *GetCachedProperties()->index;
		auto result = With<int>([](auto&& obj) { return obj.GetIndex(); });
		if (result && GetCachedProperties())
			GetCachedProperties()->index = *result;
		return result;
	}

	[[nodiscard]] inline auto GetBounds() const -> ObjectResult<SRect> {
		return With<SRect>([](auto&& obj) { return obj.GetBounds(); });
	}

	[[nodiscard]] inline auto GetApplicationName() const -> ObjectResult<std::string_view> {
		if (GetCachedProperties() && GetCachedProperties()->application_name)
			return *GetCachedProperties()->application_name;
		auto result = With<std::string_view>([](auto&& obj) { return obj.GetApplicationName(); });
		if (result && GetCachedProperties())
			GetCachedProperties()->application_name = *result;
		return result;
	}
	[[nodiscard]] inline auto GetName() const -> ObjectResult<std::string_view> {
		if (GetCachedProperties() && GetCachedProperties()->name)
			return *GetCachedProperties()->name;
		auto result = With<std::string_view>([](auto&& obj) { return obj.GetName(); });
		if (result && GetCachedProperties())
			GetCachedProperties()->name = *result;
		return result;
	}
	[[nodiscard]] inline auto GetDescription() const -> ObjectResult<std::string_view> {
		if (GetCachedProperties() && GetCachedProperties()->description)
			return *GetCachedProperties()->description;
		auto result = With<std::string_view>([](auto&& obj) { return obj.GetDescription(); });
		if (result && GetCachedProperties())
			GetCachedProperties()->description = *result;
		return result;
	}
};

export class CTextProviderProxy final : public UnknownProxy {
public:
	[[nodiscard]] inline auto GetCursor() const -> ObjectResult<int> {
		if (GetCachedProperties() && GetCachedProperties()->cursor)
			return *GetCachedProperties()->cursor;
		auto result = With<int>([](auto&& obj) { return obj.GetCursor(); });
		if (result && GetCachedProperties())
			GetCachedProperties()->cursor = *result;
		return result;
	}
	[[nodiscard]] inline auto GetText(int cursor, ETextGranularity granularity) const -> ObjectResult<STextRange> {
		return With<STextRange>([cursor, granularity](auto&& obj) { return obj.GetText(cursor, granularity); });
	}
	[[nodiscard]] inline auto GetSelected() const -> ObjectResult<STextRange> {
		return With<STextRange>([](auto&& obj) { return obj.GetSelectedText(); });
	}
};

export class CSelectionProviderProxy final : public UnknownProxy {
public:
	[[nodiscard]] inline auto GetChildrenCount() const -> ObjectResult<int> {
		return With<int>([](auto&& obj) { return obj.GetSelectedChildrenCount(); });
	}

	[[nodiscard]] inline auto GetChildAt(int index) const -> ObjectResult<CObjectProxy> {
		auto result = With<void*>([index](auto&& obj) { return obj.GetChildAt(index); });
		if (!result)
			return std::unexpected(result.error());
		return CObjectProxy(*result);
	}
};

export class CValueProviderProxy final : public UnknownProxy {
public:
	[[nodiscard]] inline auto GetMin() const -> ObjectResult<double> {
		if (GetCachedProperties() && GetCachedProperties()->min_value)
			return *GetCachedProperties()->min_value;
		auto result = With<double>([](auto&& obj) { return obj.GetMinValue(); });
		if (result && GetCachedProperties())
			GetCachedProperties()->min_value = *result;
		return result;
	}
	[[nodiscard]] inline auto GetMax() const -> ObjectResult<double> {
		if (GetCachedProperties() && GetCachedProperties()->max_value)
			return *GetCachedProperties()->max_value;
		auto result = With<double>([](auto&& obj) { return obj.GetMaxValue(); });
		if (result && GetCachedProperties())
			GetCachedProperties()->max_value = *result;
		return result;
	}
	[[nodiscard]] inline auto GetCurrent() const -> ObjectResult<double> {
		if (GetCachedProperties() && GetCachedProperties()->current_value)
			return *GetCachedProperties()->current_value;
		auto result = With<double>([](auto&& obj) { return obj.GetCurrentValue(); });
		if (result && GetCachedProperties())
			GetCachedProperties()->current_value = *result;
		return result;
	}
};

export class ActionProviderProxy final : public UnknownProxy {
public:
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
