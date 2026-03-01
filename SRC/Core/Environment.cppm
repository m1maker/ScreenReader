module;
#include "Environment.h"
#if SR_LINUX
export module Core.Environment;
import Platforms.Linux.Object;
export using CObject = CObjectAtspi;
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