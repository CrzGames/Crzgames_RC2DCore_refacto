#!/bin/bash

# Generate lib for iOS to arm64, arm64e and x86_64 (iphoneos and iphonesimulator)

mkdir -p dist/lib/ios/arm64-arm64e/iphoneos
mkdir -p dist/lib/ios/x86_64-arm64/iphonesimulator
mkdir -p dist/lib/ios/outputs/arm64-arm64e/iphoneos/Debug
mkdir -p dist/lib/ios/outputs/arm64-arm64e/iphoneos/Release
mkdir -p dist/lib/ios/outputs/x86_64-arm64/iphonesimulator/Debug
mkdir -p dist/lib/ios/outputs/x86_64-arm64/iphonesimulator/Release

cd dist/lib/ios/arm64-arm64e/iphoneos

echo -e "\e[32m \n Generate project with CMake for iOS arm64-arm64e (iphoneos)...\e[0m"
cmake ../../../../.. -G "Xcode" -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_SYSROOT=iphoneos
echo -e "\e[32m \n Build project for Release...\e[0m"
cmake --build . --config Release
echo -e "\e[32m \n Build project for Debug...\e[0m"
cmake --build . --config Debug

cd ../../../../../dist/lib/ios/x86_64-arm64/iphonesimulator

echo -e "\e[32m \n Generate project with CMake for iOS x86_64-arm64 (iphonesimulator)...\e[0m"
cmake ../../../../.. -G "Xcode" -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_SYSROOT=iphonesimulator
echo -e "\e[32m \n Build project for Release...\e[0m"
cmake --build . --config Release
echo -e "\e[32m \n Build project for Debug...\e[0m"
cmake --build . --config Debug

cd ../../../../..

cp -r dist/lib/ios/arm64-arm64e/iphoneos/Release-iphoneos/* dist/lib/ios/outputs/arm64-arm64e/iphoneos/Release
cp -r dist/lib/ios/arm64-arm64e/iphoneos/Debug-iphoneos/* dist/lib/ios/outputs/arm64-arm64e/iphoneos/Debug
cp -r dist/lib/ios/x86_64-arm64/iphonesimulator/Release-iphonesimulator/* dist/lib/ios/outputs/x86_64-arm64/iphonesimulator/Release
cp -r dist/lib/ios/x86_64-arm64/iphonesimulator/Debug-iphonesimulator/* dist/lib/ios/outputs/x86_64-arm64/iphonesimulator/Debug

rm -rf dist/lib/ios/arm64-arm64e
rm -rf dist/lib/ios/x86_64-arm64

mkdir -p dist/lib/ios/Release/arm64-arm64e/iphoneos
mkdir -p dist/lib/ios/Release/x86_64-arm64/iphonesimulator
mkdir -p dist/lib/ios/Debug/arm64-arm64e/iphoneos
mkdir -p dist/lib/ios/Debug/x86_64-arm64/iphonesimulator

cp -r dist/lib/ios/outputs/arm64-arm64e/iphoneos/Release/* dist/lib/ios/Release/arm64-arm64e/iphoneos
cp -r dist/lib/ios/outputs/arm64-arm64e/iphoneos/Debug/* dist/lib/ios/Debug/arm64-arm64e/iphoneos
cp -r dist/lib/ios/outputs/x86_64-arm64/iphonesimulator/Release/* dist/lib/ios/Release/x86_64-arm64/iphonesimulator
cp -r dist/lib/ios/outputs/x86_64-arm64/iphonesimulator/Debug/* dist/lib/ios/Debug/x86_64-arm64/iphonesimulator

rm -rf dist/lib/ios/outputs

echo -e "\033[32m \n Lib RC2D for iOS >= 12.0 x86_64/arm64 (iphonesimulator) and arm64/arm64e (iphoneos) to Release/Debug generated successfully, go to the dist/lib/ios/ directory... \n\033[0m"