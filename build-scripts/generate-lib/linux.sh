#!/bin/bash

# Generate lib for Linux to x86_64 architecture

mkdir -p build/linux/x86_64
cd build/linux/x86_64

echo -e "\e[32m \n Generate project with CMake for Linux x86_64...\e[0m"
cmake ../../.. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DRC2D_ARCH=x64 -DRC2D_PLATFORM=Linux
echo -e "\e[32m \n Build project for Debug...\e[0m"
cmake --build .

# Final message
echo -e "\033[32m \n Generate lib RC2D for Linux x86_64 to Release/Debug generated successfully, go to the dist/lib/linux/x86_64 directory... \n\033[0m"
