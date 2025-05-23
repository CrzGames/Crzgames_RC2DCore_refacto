#!/bin/bash

echo -e "\e[32m\nGenerating Unix Makefiles project for SLR3-Sniper x64...\e[0m"

for build_type in Debug Release; do
  echo -e "\e[32m\nBuilding $build_type...\e[0m"
  cmake -S . -B build/linux/x64/$build_type \
    -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=$build_type \
    -DRC2D_ARCH=x64 \
    -DRC2D_PLATFORM=SLR3-SNIPER
  cmake --build build/linux/x64/$build_type
done

# Final message
echo -e "\033[32m \n Generate lib RC2D for Linux x64 to Release/Debug generated successfully, go to the dist/lib/linux/x64 directory... \n\033[0m"
