#pragma once
#include <optional>

#define DeclareCache(T, name) std::optional<T> name

#define Cache(name, value) name = value
#define ReturnCache(name)\
do {\
	if (name.has_value()) {\
		return name.value();\
	}\
}\
while(0)

#define CacheReturn(name, value_to_cache)\
do {\
	name = value_to_cache;\
	return name.value();\
}\
while(0)

