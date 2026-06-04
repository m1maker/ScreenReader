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
#include "Environment.h"

#include <variant>
#if SR_LINUX
export module Core.Environment;
export import Platforms.Linux.Object;
export import Platforms.Linux.Worker;
export using DefaultPlatformDependentWorker = CPlatformDependentWorkerLinux;
export using ObjectVariant = std::variant<std::monostate, CObjectAtspi>;
export using ProviderVariant = ObjectVariant;
export using ActionProviderVariant = ObjectVariant;
export using TextProviderVariant = ObjectVariant;
export using SelectionProviderVariant = ObjectVariant;
export using ValueProviderVariant = ObjectVariant;
#else
export module Core.Environment;
#endif

export namespace Environment {
#if SR_WIN
constexpr bool IsWindows = true;
#else
constexpr bool IsWindows = false;
#endif

#if SR_APPLE
constexpr bool IsApple = true;
#if SR_IOS
constexpr bool IsIos = true;
constexpr bool IsMac = false;
#else
constexpr bool IsIos = false;
constexpr bool IsMac = true;
#endif
#else
constexpr bool IsApple = false;
constexpr bool IsIos = false;
constexpr bool IsMac = false;
#endif

#if SR_LINUX
constexpr bool IsLinux = true;
#else
constexpr bool IsLinux = false;
#endif

#if SR_ANDROID
constexpr bool IsAndroid = true;
#else
constexpr bool IsAndroid = false;
#endif
} // namespace Environment