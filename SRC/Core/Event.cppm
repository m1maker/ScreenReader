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

// A variant for abstracting event types. Categories are declared here.
module;
#include <memory>
#include <memory_resource>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
export module Core.Event;
import Core.Environment;
import Core.KeyInfo;
import Core.Object;
import Proxies.Object;

/*
The object event category.
Events such as changes to an object's focus, state, name, description, value, etc., always dispatch a CObjectEvent.
*/
export class CObjectEvent final {
public:
	EObjectEventType type;
	CObjectProxy object;
};

/*
The keyboard event category.
Key pressed and released events. always dispatch a CKeyboardEvent.
*/
export class CKeyboardEvent final {
public:
	enum eKeyboardEventType : unsigned char { NONE = 0, KEY_PRESSED, KEY_RELEASED } type;

	EKeycode keycode;
};

namespace std {
export template <> struct hash<SHotkeyInfo> {
	auto operator()(const SHotkeyInfo& k) const noexcept -> std::size_t { return std::hash<uint32_t>{}(k.Pack()); }
};
} // namespace std

export using EventVariant = std::variant<std::monostate, CObjectEvent, CKeyboardEvent>;

export class CEvent final {
	EventVariant m_variant;

public:
	using allocator_type = std::pmr::polymorphic_allocator<>;

	~CEvent() = default;
	CEvent(CObjectEvent&& object_event, allocator_type alloc = {}) : m_variant(std::move(object_event)) {}

	CEvent(CKeyboardEvent&& keyboard_event, allocator_type alloc = {}) : m_variant(std::move(keyboard_event)) {}

	CEvent(CEvent&& other, allocator_type alloc) : m_variant(std::move(other.m_variant)) {}

	CEvent(CEvent&&) = default;
	auto operator=(CEvent&&) -> CEvent& = default;

	operator EventVariant() const noexcept { return m_variant; }
};
