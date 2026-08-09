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
#include <thread>
#include <utility>
#include <variant>
export module Proxies.Object;
import Core.Environment;
import Core.Object;
import Core.ObjectCache;
import Core.ObjectProviderMeta;
import Core.Rect;
import Core.Text;
import Core.Timer;
import Traits.AtomicRefCountedObject;

class UnknownProxy : /*protected*/ public TAtomicRefCountedObject<UnknownProxy, SCachedObjectData> {
	template <EObjectFetchValue Value> using FetchValueType = TObjectFetchValue<Value>::type;

protected:
	UnknownProxy() = default;
	explicit UnknownProxy(void* memory) : TAtomicRefCountedObject(memory) {}

	auto ApplyFetchMode(EObjectFetchMode mode, uint64_t timeout_ms = 0) const noexcept -> EObjectError {
		using enum EObjectFetchMode;
		switch (mode) {
		case ASYNC:
			return EObjectError::BUSY;
		case AWAIT:
			return AwaitAndPull();
		case AWAIT_WITH_TIMEOUT:
			return AwaitAndPull(timeout_ms);
		case UNKNOWN:
			break;
		}
		return EObjectError::INVALID_ARGUMENTS;
	}

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
		GetInactiveSlot()->busy.test_and_set(std::memory_order_release);
		GetInactiveSlot()->pending_requests.fetch_add(1, std::memory_order_relaxed);
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

	auto TryPull() const noexcept -> EObjectError {
		if (!GetData()->wants_to_switch.test(std::memory_order_acquire))
			return EObjectError::NOTHING_TO_FETCH;
		auto inactive_slot = GetInactiveSlot();
		if (!inactive_slot || inactive_slot->busy.test(std::memory_order_acquire))
			return EObjectError::BUSY;
		GetData()->current_slot.store(GetInactiveSlotNumber(), std::memory_order_release);
		GetData()->wants_to_switch.clear(std::memory_order_release);
		return EObjectError::SUCCESS;
	}

	auto AwaitAndPull() const noexcept -> EObjectError {
		if (!GetData()->wants_to_switch.test(std::memory_order_acquire))
			return EObjectError::NOTHING_TO_FETCH;
		auto inactive_slot = GetInactiveSlot();
		if (!inactive_slot)
			return EObjectError::FETCH_SLOT_DEFUNCT;
		inactive_slot->busy.wait(true, std::memory_order_acquire);
		GetData()->current_slot.store(GetInactiveSlotNumber(), std::memory_order_release);
		GetData()->wants_to_switch.clear(std::memory_order_release);
		return EObjectError::SUCCESS;
	}
	auto AwaitAndPull(uint64_t timeout_ms) const noexcept -> EObjectError {
		if (!GetData()->wants_to_switch.test(std::memory_order_acquire))
			return EObjectError::NOTHING_TO_FETCH;
		auto inactive_slot = GetInactiveSlot();
		if (!inactive_slot)
			return EObjectError::FETCH_SLOT_DEFUNCT;

		// Unfortunately there is no built-in atomic_flag wait until/for method.
		CTimer wait_timer;
		while (inactive_slot->busy.test(std::memory_order_acquire)) {
			if (wait_timer.Elapsed() > timeout_ms)
				return EObjectError::TIMEOUT;
			std::this_thread::yield();
		}
		GetData()->current_slot.store(GetInactiveSlotNumber(), std::memory_order_release);
		GetData()->wants_to_switch.clear(std::memory_order_release);
		return EObjectError::SUCCESS;
	}

	inline auto Fetch(ObjectFetchMask mask, EObjectFetchMode mode, uint64_t timeout_ms) const noexcept -> EObjectError {
		PushFetchRequest(mask);
		return ApplyFetchMode(mode, timeout_ms);
	}
	inline auto Fetch(ObjectFetchMask mask /*, void*/) const noexcept -> EObjectError {
		return Fetch(mask,
			GetData()->fetch_mode.load(std::memory_order_relaxed),
			GetData()->fetch_timeout_ms.load(std::memory_order_relaxed));
	}

	template <typename Provider> [[nodiscard]] auto GetAs() const -> Provider { return Provider(*this); }

	bool IsValid() const noexcept { return GetRef() > 0 && GetData()->native_handle; }
	[[nodiscard]] auto operator==(const UnknownProxy& other) const noexcept { return GetData() == other.GetData(); }

	inline void SetFetchMode(EObjectFetchMode mode = cObjectFetchRequestDefaultMode,
		uint64_t timeout_ms = cObjectFetchRequestDefaultTimeoutMs) const noexcept {
		GetData()->fetch_mode.store(mode, std::memory_order_relaxed);
		GetData()->fetch_timeout_ms.store(timeout_ms, std::memory_order_relaxed);
	}
	inline void ResetFetchModeToConstexpr() const noexcept { SetFetchMode(); }

	template <EObjectFetchValue Value>
	[[nodiscard]] auto GetValue() const noexcept -> ObjectResult<FetchValueType<Value>> {
		auto active_slot = GetActiveSlot();
		if (!active_slot) [[unlikely]] {
			return std::unexpected(EObjectError::FETCH_SLOT_DEFUNCT);
		}

		if (!active_slot->mask.test(std::to_underlying(Value))) {
			if (!GetData()->lazy_fetch.test(std::memory_order_relaxed)) {
				return std::unexpected(EObjectError::MANUAL_FETCH_REQUIRED);
			}
			auto error = Fetch(ObjectFetchMask(std::to_underlying(Value)));
			if (error != EObjectError::SUCCESS) {
				return std::unexpected(error);
			}

			active_slot = GetActiveSlot();
		}

		using enum EObjectFetchValue;
		// It's sad that we don't have something like constexpr switch.
		if constexpr (Value == TYPE)
			return active_slot->type;
		else if constexpr (Value == STATES)
			return active_slot->states;
		else if constexpr (Value == CAPABILITIES)
			return active_slot->capabilities;
		else if constexpr (Value == PARENT)
			return active_slot->parent;
		else if constexpr (Value == CHILDREN)
			return active_slot->children;
		else if constexpr (Value == SELECTED_CHILDREN)
			return active_slot->selected_children;
		else if constexpr (Value == INDEX)
			return active_slot->index;
		else if constexpr (Value == BOUNDS)
			return active_slot->bounds;
		else if constexpr (Value == TOOLKIT_NAME)
			return active_slot->toolkit_name;
		else if constexpr (Value == TOOLKIT_VERSION)
			return active_slot->toolkit_version;
		else if constexpr (Value == NAME)
			return active_slot->name;
		else if constexpr (Value == DESCRIPTION)
			return active_slot->description;
		else if constexpr (Value == HELP_TEXT)
			return active_slot->help_text;

		return std::unexpected(EObjectError::FAIL);
	}
};

export class CObjectProxy final : public UnknownProxy {
	using enum EObjectFetchValue;

public:
	CObjectProxy() = default;
	explicit CObjectProxy(void* memory) : UnknownProxy(memory) {}

	auto Fetch(void) const noexcept -> EObjectError {
		return UnknownProxy::Fetch(GetObjectProviderValueMask(EObjectProvider::MAIN));
	}

	[[nodiscard]] inline auto GetType() const -> ObjectResult<EObjectType> { return GetValue<TYPE>(); }
	[[nodiscard]] inline auto GetStates() const -> ObjectResult<ObjectStateMask> { return GetValue<STATES>(); }
	[[deprecated("Use 'GetStates' instead"), nodiscard]] inline auto GetState() const { return GetStates(); }

	[[nodiscard]] inline auto GetCapabilities() const -> ObjectResult<ObjectCapabilityMask> {
		return GetValue<CAPABILITIES>();
	}

	[[nodiscard]] inline auto GetParent() const -> ObjectResult<CObjectProxy> {
		auto parent = GetValue<PARENT>();
		if (!parent)
			return std::unexpected(parent.error());
		return CObjectProxy(*parent);
	}

	[[nodiscard]] inline auto GetChildrenCount() const -> ObjectResult<int> {
		auto children = GetValue<CHILDREN>();
		if (!children)
			return std::unexpected(children.error());
		return children->size();
	}

	[[nodiscard]] inline auto GetChildAt(int index) const -> ObjectResult<CObjectProxy> {
		auto children = GetValue<CHILDREN>();
		if (!children)
			return std::unexpected(children.error());
		else if (index >= children->size() || index < 0) [[unlikely]]
			return std::unexpected(EObjectError::INVALID_ARGUMENTS);
		return CObjectProxy(children->operator[](index));
	}
	[[nodiscard]] inline auto GetIndex() const -> ObjectResult<int> { return GetValue<INDEX>(); }

	[[nodiscard]] inline auto GetBounds() const -> ObjectResult<SRect> { return GetValue<BOUNDS>(); }

	[[nodiscard]] inline auto GetApplicationName() const -> ObjectResult<std::string_view> {
		return GetValue<TOOLKIT_NAME>();
	}
	[[nodiscard]] inline auto GetName() const -> ObjectResult<std::string_view> { return GetValue<NAME>(); }
	[[nodiscard]] inline auto GetDescription() const -> ObjectResult<std::string_view> {
		return GetValue<DESCRIPTION>();
	}
	[[nodiscard]] inline auto GetHelpText() const -> ObjectResult<std::string_view> { return GetValue<HELP_TEXT>(); }
};

export class CTextProviderProxy final : public UnknownProxy {
public:
	void Fetch() const noexcept { PushFetchRequest(GetObjectProviderValueMask(EObjectProvider::TEXT)); }

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
	void Fetch() const noexcept { PushFetchRequest(GetObjectProviderValueMask(EObjectProvider::SELECTION)); }

	[[nodiscard]] inline auto GetChildrenCount() const -> ObjectResult<int> {
		auto selected_children = GetActiveSlot()->selected_children;
		if (!selected_children)
			return std::unexpected(selected_children.error());
		return selected_children->size();
	}

	[[nodiscard]] inline auto GetChildAt(int index) const -> ObjectResult<CObjectProxy> {
		auto selected_children = GetActiveSlot()->selected_children;
		if (!selected_children)
			return std::unexpected(selected_children.error());
		else if (index >= selected_children->size() || index < 0) [[unlikely]]
			return std::unexpected(EObjectError::INVALID_ARGUMENTS);
		return CObjectProxy(selected_children->operator[](index));
	}
};

export class CValueProviderProxy final : public UnknownProxy {
public:
	void Fetch() const noexcept { PushFetchRequest(GetObjectProviderValueMask(EObjectProvider::VALUE)); }

	[[nodiscard]] inline auto GetMin() const -> ObjectResult<double> { return GetActiveSlot()->value_min; }
	[[nodiscard]] inline auto GetMax() const -> ObjectResult<double> { return GetActiveSlot()->value_max; }
	[[nodiscard]] inline auto GetCurrent() const -> ObjectResult<double> { return GetActiveSlot()->value_current; }
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
