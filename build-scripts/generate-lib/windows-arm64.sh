#!/bin/bash

echo -e "\e[32m\nGenerating Visual Studio 2022 project for Windows arm64...\e[0m"

cmake -S . -B build/windows/arm64 \
  -G "Visual Studio 17 2022" \
  -A ARM64

for build_type in Debug Release; do
  echo -e "\e[32m\nBuilding $build_type...\e[0m"
  cmake --build build/windows/arm64 --config $build_type
done

echo -e "\033[32m\nLib RC2D for Windows arm64 built successfully for Debug and Release.\nGo to dist/lib/windows/arm64.\n\033[0m"
