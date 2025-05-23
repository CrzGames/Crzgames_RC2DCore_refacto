#!/bin/bash

echo -e "\e[32m\nGenerating Visual Studio 2022 project for Windows x64...\e[0m"

cmake -S . -B build/windows/x64 \
  -G "Visual Studio 17 2022" \
  -A x64 \
  -DRC2D_ARCH=x64

for build_type in Debug Release; do
  echo -e "\e[32m\nBuilding $build_type...\e[0m"
  cmake --build build/windows/x64 --config $build_type
done

echo -e "\033[32m\nLib RC2D for Windows x64 built successfully for Debug and Release.\nGo to dist/lib/windows/x64.\n\033[0m"
