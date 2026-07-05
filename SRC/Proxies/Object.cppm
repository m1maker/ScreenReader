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

public:
	// TNonAtomicRefCountedObject

	void do_OnDestroy() noexcept {
		/*
				static_cast<void>(With<>([](auto&& obj) {
					obj.OnDestroy();
					return ObjectResult<>();
				}));
		*/
	}

	template <typename Provider> [[nodiscard]] auto GetAs() const -> Provider { return Provider(*this); }

	bool IsValid() const { return false; }
	[[nodiscard]] auto operator==(const UnknownProxy& other) const noexcept { return GetData() == other.GetData(); }
};

export class CObjectProxy final : public UnknownProxy {
public:
	CObjectProxy() = default;
	explicit CObjectProxy(void* memory) : UnknownProxy(memory) {}

	[[nodiscard]] inline auto GetType() const -> ObjectResult<EObjectType> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}
	[[nodiscard]] inline auto GetState() const -> ObjectResult<ObjectStateMask> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}
	[[nodiscard]] inline auto GetCapabilities() const -> ObjectResult<ObjectCapabilityMask> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}

	[[nodiscard]] inline auto GetParent() const -> ObjectResult<CObjectProxy> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}

	[[nodiscard]] inline auto GetChildrenCount() const -> ObjectResult<int> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}

	[[nodiscard]] inline auto GetChildAt(int index) const -> ObjectResult<CObjectProxy> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}
	[[nodiscard]] inline auto GetIndex() const -> ObjectResult<int> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}

	[[nodiscard]] inline auto GetBounds() const -> ObjectResult<SRect> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}

	[[nodiscard]] inline auto GetApplicationName() const -> ObjectResult<std::string_view> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}
	[[nodiscard]] inline auto GetName() const -> ObjectResult<std::string_view> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}
	[[nodiscard]] inline auto GetDescription() const -> ObjectResult<std::string_view> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}
	[[nodiscard]] inline auto GetHelpText() const -> ObjectResult<std::string_view> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}
};

export class CTextProviderProxy final : public UnknownProxy {
public:
	[[nodiscard]] inline auto GetCursor() const -> ObjectResult<int> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}
	[[nodiscard]] inline auto GetText(int cursor, ETextGranularity granularity) const -> ObjectResult<STextRange> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}
	[[nodiscard]] inline auto GetSelected() const -> ObjectResult<STextRange> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}
};

export class CSelectionProviderProxy final : public UnknownProxy {
public:
	[[nodiscard]] inline auto GetChildrenCount() const -> ObjectResult<int> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}

	[[nodiscard]] inline auto GetChildAt(int index) const -> ObjectResult<CObjectProxy> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}
};

export class CValueProviderProxy final : public UnknownProxy {
public:
	[[nodiscard]] inline auto GetMin() const -> ObjectResult<double> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}
	[[nodiscard]] inline auto GetMax() const -> ObjectResult<double> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}
	[[nodiscard]] inline auto GetCurrent() const -> ObjectResult<double> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}
};

export class ActionProviderProxy final : public UnknownProxy {
public:
	[[nodiscard]] inline auto GetCount() const -> ObjectResult<int> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}

	[[nodiscard]] inline auto GetType(int number) const -> ObjectResult<EObjectAction> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}
	[[nodiscard]] inline auto GetName(int number) const -> ObjectResult<std::string_view> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}
	[[nodiscard]] inline auto Do(int number) -> ObjectResult<> { return std::unexpected(EObjectError::NOT_SUPPORTED); }
};

export class CRelationProviderProxy final : public UnknownProxy {
public:
	[[nodiscard]] inline auto GetCount() const -> ObjectResult<int> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}
	[[nodiscard]] inline auto GetType(int index) const -> ObjectResult<EObjectRelationType> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}
	[[nodiscard]] inline auto GetTargetCount(int index) const -> ObjectResult<int> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}
	[[nodiscard]] inline auto GetTarget(int relation_index, int target_index) const -> ObjectResult<CObjectProxy> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}
};
