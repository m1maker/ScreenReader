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
