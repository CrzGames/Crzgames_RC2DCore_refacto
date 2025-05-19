#!/bin/bash

# Generate lib for macOS to Intel x64 and Apple Silicon arm64 architectures

mkdir -p dist/lib/macos/arm64/
mkdir -p dist/lib/macos/outputs/x64-arm64/Debug
mkdir -p dist/lib/macos/outputs/x64-arm64/Release

cd dist/lib/macos/arm64/

echo -e "\e[32m \n Generate project with CMake for macOS Apple Silicon arm64...\e[0m"
cmake ../../../.. -G "Xcode" CMAKE_OSX_SYSROOT="macosx"
echo -e "\e[32m \n Build project for Release...\e[0m"
cmake --build . --config Release
echo -e "\e[32m \n Build project for Debug...\e[0m"
cmake --build . --config Debug

cd ../../../..

cp -r dist/lib/macos/x64-arm64/Release/* dist/lib/macos/outputs/x64-arm64/Release
cp -r dist/lib/macos/x64-arm64/Debug/* dist/lib/macos/outputs/x64-arm64/Debug

rm -rf dist/lib/macos/x64-arm64

mkdir -p dist/lib/macos/Release/x64-arm64
mkdir -p dist/lib/macos/Debug/x64-arm64

cp -r dist/lib/macos/outputs/x64-arm64/Release/* dist/lib/macos/Release/x64-arm64
cp -r dist/lib/macos/outputs/x64-arm64/Debug/* dist/lib/macos/Debug/x64-arm64

rm -rf dist/lib/macos/outputs

echo -e "\033[32m \n Lib RC2D for macOS >= 11.0 to x86_64 Intel and Apple Silicon arm64 to Debug/Release generated successfully, go to the dist/lib/macos/ directory... \n\033[0m"