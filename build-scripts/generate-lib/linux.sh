#!/bin/bash

# Generate lib for Linux to x86_64 architecture

mkdir -p dist/lib/linux/x86_64
mkdir -p dist/lib/linux/outputs/x86_64/Debug
mkdir -p dist/lib/linux/outputs/x86_64/Release

# Build for Debug
mkdir -p dist/lib/linux/x86_64/Debug
cd dist/lib/linux/x86_64/Debug

echo -e "\e[32m \n Generate project with CMake for Linux x86_64...\e[0m"
cmake ../../../../../ -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_SYSTEM_NAME=Linux
echo -e "\e[32m \n Build project for Debug...\e[0m"
cmake --build .

# Return to the base directory
cd ../../../../..

# Build for Release
mkdir -p dist/lib/linux/x86_64/Release
cd dist/lib/linux/x86_64/Release

echo -e "\e[32m \n Generate project with CMake for Linux x86_64...\e[0m"
cmake ../../../../.. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_SYSTEM_NAME=Linux 
echo -e "\e[32m \n Build project for Release...\e[0m"
cmake --build .

# Return to the base directory
cd ../../../../..

cp -r dist/lib/linux/x86_64/Debug/*.a dist/lib/linux/outputs/x86_64/Debug
cp -r dist/lib/linux/x86_64/Release/*.a dist/lib/linux/outputs/x86_64/Release

rm -rf dist/lib/linux/x86_64

mkdir -p dist/lib/linux/Release/x86_64
mkdir -p dist/lib/linux/Debug/x86_64

cp -r dist/lib/linux/outputs/x86_64/Release/*.a dist/lib/linux/Release/x86_64
cp -r dist/lib/linux/outputs/x86_64/Debug/*.a dist/lib/linux/Debug/x86_64

rm -rf dist/lib/linux/outputs

# Final message
echo -e "\033[32m \n Generate lib RC2D for Linux x86_64 to Release/Debug generated successfully, go to the dist/lib/linux/x86_64 directory... \n\033[0m"
