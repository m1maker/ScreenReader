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

#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define SR_WIN 1
#else
#define SR_WIN 0
#endif

#if defined(__APPLE__)
#include <TargetConditionals.h>
#define SR_APPLE 1
#if TARGET_OS_IPHONE
#define SR_IOS 1
#define SR_MAC 0
#else
#define SR_IOS 0
#define SR_MAC 1
#endif
#else
#define SR_APPLE 0
#define SR_MAC 0
#define SR_IOS 0
#endif

#if defined(__linux__)
#if defined(__ANDROID__)
#define SR_ANDROID 1
#define SR_LINUX 0
#else
#define SR_ANDROID 0
#define SR_LINUX 1
#endif
#else
#define SR_LINUX 0
#define SR_ANDROID 0
#endif

#define SR_BEGIN namespace Mmade::ScreenReader {
#define SR_END }
