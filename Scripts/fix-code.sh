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

