#!/bin/bash

# Generate lib for Android to arm64-v8a and armeabi-v7a architectures

# Vérifier le système d'exploitation
if [[ "$OSTYPE" == "linux-gnu" || "$OSTYPE" == "darwin"* ]]; then
    # Système Unix
    GRADLE="./gradlew"
else
    # Système Windows
    GRADLE="gradle"
fi

# Define base directories
BASE_BUILD_DIR_RELEASE="android-project/app/build/intermediates/cmake/release/obj"
DIST_DIR_RELEASE="dist/lib/android/Release"
BASE_BUILD_DIR_DEBUG="android-project/app/build/intermediates/cmake/debug/obj"
DIST_DIR_DEBUG="dist/lib/android/Debug"

# Create destination directories
mkdir -p "$DIST_DIR_RELEASE/arm64-v8a"
mkdir -p "$DIST_DIR_RELEASE/armeabi-v7a"
mkdir -p "$DIST_DIR_DEBUG/arm64-v8a"
mkdir -p "$DIST_DIR_DEBUG/armeabi-v7a"

# Vérifier si ANDROID_HOME est défini
if [ -z "$ANDROID_HOME" ]; then
    echo "Erreur : ANDROID_HOME n'est pas défini."
    echo "Veuillez définir ANDROID_HOME pour pointer vers votre répertoire NDK Android."
    echo "Exemple : ANDROID_HOME=C:\Users\Corentin\AppData\Local\Android\Sdk\ndk\25.2.9519653"
    exit 1
fi

# Change to project directory
cd android-project/

# Clean and build the project
echo -e "\e[32m \n Clean project...\e[0m"
$GRADLE clean

echo -e "\e[32m \n Build project for Release...\e[0m"
$GRADLE assembleRelease
echo -e "\e[32m \n Build project for Debug...\e[0m"
$GRADLE assembleDebug

cd ../

# Copy .so files to respective directories
cp "$BASE_BUILD_DIR_RELEASE/arm64-v8a/librc2d.so" "$DIST_DIR_RELEASE/arm64-v8a/"
cp "$BASE_BUILD_DIR_RELEASE/armeabi-v7a/librc2d.so" "$DIST_DIR_RELEASE/armeabi-v7a/"

cp "$BASE_BUILD_DIR_DEBUG/arm64-v8a/librc2d.so" "$DIST_DIR_DEBUG/arm64-v8a/"
cp "$BASE_BUILD_DIR_DEBUG/armeabi-v7a/librc2d.so" "$DIST_DIR_DEBUG/armeabi-v7a/"

echo -e "\e[32m \n Lib RC2D for Android >= 9.0 for arm64-v8a and armeabi-v7a architectures generated successfully, go to dist/lib/android directory...\e[0m"