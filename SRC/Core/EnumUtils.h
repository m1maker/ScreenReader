#pragma once
#include <type_traits>

#define EnableBitwiseEnum(EnumType) \
[[nodiscard]] constexpr inline EnumType operator|(EnumType lhs, EnumType rhs) noexcept { \
	return static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(lhs) | \
		static_cast<std::underlying_type_t<EnumType>>(rhs)); \
} \
inline constexpr EnumType& operator|=(EnumType& lhs, EnumType rhs) noexcept { \
	lhs = lhs | rhs; \
	return lhs; \
} \
[[nodiscard]] constexpr inline EnumType operator&(EnumType lhs, EnumType rhs) noexcept { \
	return static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(lhs) & \
		static_cast<std::underlying_type_t<EnumType>>(rhs)); \
} \
inline constexpr EnumType& operator&=(EnumType& lhs, EnumType rhs) noexcept { \
	lhs = lhs & rhs; \
	return lhs; \
} \
[[nodiscard]] constexpr inline EnumType operator~(EnumType val) noexcept { \
	return static_cast<EnumType>(~static_cast<std::underlying_type_t<EnumType>>(val)); \
} \
[[nodiscard]] constexpr inline bool operator!(EnumType val) noexcept { \
	return static_cast<std::underlying_type_t<EnumType>>(val) == 0; \
}
