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
#include <string_view>
export module Core.Device;

export enum class EDeviceType : unsigned char { NO = 0, KEYBOARD, MOUSE, TOUCHSCREEN };

export [[nodiscard]] constexpr auto GetDeviceName(EDeviceType d) noexcept -> std::string_view {
	switch (d) {
	case EDeviceType::KEYBOARD:
		return "Keyboard";
	case EDeviceType::MOUSE:
		return "Mouse";
	case EDeviceType::TOUCHSCREEN:
		return "Touchscreen";
	default:
		return "Unknown";
	}
}
