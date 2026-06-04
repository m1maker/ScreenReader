#
# This file is part of the Screen Reader project.
#
# Copyright (C) 2025-2026  Georgiy Bondarenko (M_maker / m1maker) <georgijbondarenko248@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

#!/bin/bash
set -e

if [ ! -f build/compile_commands.json ]; then
    mkdir -p build
    cd build
    cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
    cd ..
fi

clang-tidy --fix --fix-errors \
  -p build/ \
  -checks='performance-*,bugprone-*,cert-*,cppcoreguidelines-*,modernize-*' \
  $(find . -name "*.h" -o -name "*.cpp")

