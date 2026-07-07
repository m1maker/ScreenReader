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
#include <atomic>
#include <expected>
#include <optional>
#include <string_view>
#include <variant>
export module Proxies.Object;
import Core.Environment;
import Core.Object;
import Core.ObjectCache;
import Core.ObjectProviderMeta;
import Core.Rect;
import Core.Text;
import Traits.AtomicRefCountedObject;

class UnknownProxy : /*protected*/ public TAtomicRefCountedObject<UnknownProxy, SCachedObjectData> {
protected:
	UnknownProxy() = default;
	explicit UnknownProxy(void* memory) : TAtomicRefCountedObject(memory) {}

	[[nodiscard]] auto GetInactiveSlotNumber() const noexcept -> unsigned char {
		return (GetData()->current_slot.load(std::memory_order_acquire) + 1) & 1;
	}
	[[nodiscard]] auto GetInactiveSlot() const noexcept -> SObjectFetchResult* {
		return &GetData()->slots[GetInactiveSlotNumber()];
	}
	[[nodiscard]] auto GetActiveSlotNumber() const noexcept -> unsigned char {
		return GetData()->current_slot.load(std::memory_order_acquire);
	}
	[[nodiscard]] auto GetActiveSlot() const noexcept -> SObjectFetchResult* {
		return &GetData()->slots[GetActiveSlotNumber()];
	}

	void PushFetchRequest(ObjectFetchMask values) const {
		ObjectFetchQueue::GetInstance().Push(SObjectFetchRequest{GetNativeHandle(), GetInactiveSlot(), values});
		GetData()->wants_to_switch.test_and_set(std::memory_order_release);
	}

public:
	// TAtomicRefCountedObject
	void do_OnDestroy() noexcept {
		/*
				static_cast<void>(With<>([](auto&& obj) {
					obj.OnDestroy();
					return ObjectResult<>();
				}));
		*/
	}

	[[nodiscard]] auto GetNativeHandle() const noexcept -> void* { return GetData()->native_handle; }

	void TryPull() const noexcept {
		if (!GetData()->wants_to_switch.test(std::memory_order_acquire))
			return;
		auto inactive_slot = GetInactiveSlot();
		if (!inactive_slot || inactive_slot->busy.test(std::memory_order_acquire))
			return;
		GetData()->current_slot.store(GetInactiveSlotNumber(), std::memory_order_release);
		GetData()->wants_to_switch.clear(std::memory_order_release);
	}

	void AwaitAndPull() const noexcept {
		if (!GetData()->wants_to_switch.test(std::memory_order_acquire))
			return;
		auto inactive_slot = GetInactiveSlot();
		if (!inactive_slot)
			return;
		inactive_slot->busy.wait(true, std::memory_order_acquire);
		GetData()->current_slot.store(GetInactiveSlotNumber(), std::memory_order_release);
		GetData()->wants_to_switch.clear(std::memory_order_release);
	}

	template <typename Provider> [[nodiscard]] auto GetAs() const -> Provider { return Provider(*this); }

	bool IsValid() const noexcept { return GetRef() > 0 && GetData()->native_handle; }
	[[nodiscard]] auto operator==(const UnknownProxy& other) const noexcept { return GetData() == other.GetData(); }
};

export class CObjectProxy final : public UnknownProxy {
public:
	CObjectProxy() = default;
	explicit CObjectProxy(void* memory) : UnknownProxy(memory) {}

	void Fetch() const noexcept { PushFetchRequest(GetObjectProviderValueMask(EObjectProvider::MAIN)); }

	[[nodiscard]] inline auto GetType() const -> ObjectResult<EObjectType> { return GetActiveSlot()->type; }
	[[nodiscard]] inline auto GetState() const -> ObjectResult<ObjectStateMask> { return GetActiveSlot()->states; }
	[[nodiscard]] inline auto GetCapabilities() const -> ObjectResult<ObjectCapabilityMask> {
		return GetActiveSlot()->capabilities;
	}

	[[nodiscard]] inline auto GetParent() const -> ObjectResult<CObjectProxy> {
		auto parent = GetActiveSlot()->parent;
		if (!parent)
			return std::unexpected(parent.error());
		return CObjectProxy(*parent);
	}

	[[nodiscard]] inline auto GetChildrenCount() const -> ObjectResult<int> {
		auto children = GetActiveSlot()->children;
		if (!children)
			return std::unexpected(children.error());
		return children->size();
	}

	[[nodiscard]] inline auto GetChildAt(int index) const -> ObjectResult<CObjectProxy> {
		auto children = GetActiveSlot()->children;
		if (!children)
			return std::unexpected(children.error());
		else if (index >= children->size() || index < 0) [[unlikely]]
			return std::unexpected(EObjectError::INVALID_ARGUMENTS);
		return CObjectProxy(children->operator[](index));
	}
	[[nodiscard]] inline auto GetIndex() const -> ObjectResult<int> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}

	[[nodiscard]] inline auto GetBounds() const -> ObjectResult<SRect> {
		return std::unexpected(EObjectError::NOT_SUPPORTED);
	}

	[[nodiscard]] inline auto GetApplicationName() const -> ObjectResult<std::string_view> {
		return GetActiveSlot()->toolkit_name;
	}
	[[nodiscard]] inline auto GetName() const -> ObjectResult<std::string_view> { return GetActiveSlot()->name; }
	[[nodiscard]] inline auto GetDescription() const -> ObjectResult<std::string_view> {
		return GetActiveSlot()->description;
	}
	[[nodiscard]] inline auto GetHelpText() const -> ObjectResult<std::string_view> {
		return GetActiveSlot()->help_text;
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
