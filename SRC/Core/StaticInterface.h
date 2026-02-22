#pragma once
#include <type_traits>

#define BindStaticInterface(DerivedType) \
auto Impl() -> DerivedType& { return *static_cast<DerivedType*>(this); } \
auto Impl() const -> const DerivedType& { return *static_cast<const DerivedType*>(this); } \
friend DerivedType;

#define ProxyMethod(Name) \
template<typename... Args> \
auto Name(Args&&... args) -> decltype(auto) { \
	return Impl().do_##Name(std::forward<Args>(args)...); \
}
