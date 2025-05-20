#!/bin/bash

# Generate lib for macOS to Intel x64 and Apple Silicon arm64 architectures

mkdir -p build/macos/arm64

cd build/macos/arm64

echo -e "\e[32m \n Generate project with CMake for macOS Apple Silicon arm64...\e[0m"
cmake ../../.. -G "Xcode" -DCMAKE_OSX_SYSROOT="macosx"
echo -e "\e[32m \n Build project for Release...\e[0m"
cmake --build . --config Release

echo -e "\033[32m \n Lib RC2D for macOS >= 11.0 to x86_64 Intel and Apple Silicon arm64 to Debug/Release generated successfully, go to the dist/lib/macos/ directory... \n\033[0m"