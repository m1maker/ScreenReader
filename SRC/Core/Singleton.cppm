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

export module Core.Singleton;

export enum class ESingletonConstructorAccessModifier : unsigned char { PROTECTED, PRIVATE };

export template <class T, ESingletonConstructorAccessModifier Modifier = ESingletonConstructorAccessModifier::PROTECTED>
class TSingleton {
public:
	static auto GetInstance() noexcept(noexcept(T())) -> T& {
		static T instance;
		return instance;
	}

	TSingleton(const TSingleton&) = delete;
	auto operator=(const TSingleton&) -> TSingleton& = delete;
	TSingleton(TSingleton&&) = delete;
	auto operator=(TSingleton&&) -> TSingleton& = delete;

protected:
	TSingleton() = default;
	~TSingleton() = default;
};

template <class T>
class TSingleton<T, ESingletonConstructorAccessModifier::PRIVATE> final
	: TSingleton<T, ESingletonConstructorAccessModifier::PROTECTED> {
	TSingleton() = delete;
	~TSingleton() = delete;

public:
	using TSingleton<T, ESingletonConstructorAccessModifier::PROTECTED>::GetInstance;
};
