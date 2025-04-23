#!/bin/bash

set -e

echo -e "\e[34m\n=== RC2D | Windows Build Script (x64 + arm64) ===\e[0m"

# Définir les chemins
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="$ROOT_DIR/build/windows"
DIST_DIR="$ROOT_DIR/dist/lib/windows"

# Architectures cibles
ARCHS=("x64")
# ARCHS=("x64" "arm64")

# Créer les dossiers nécessaires
for ARCH in "${ARCHS[@]}"; do
  mkdir -p "$BUILD_DIR/$ARCH"
  mkdir -p "$DIST_DIR/Release/$ARCH"
  mkdir -p "$DIST_DIR/Debug/$ARCH"
done

# Boucle sur chaque archi
for ARCH in "${ARCHS[@]}"; do
  echo -e "\e[32m\n▶ Generating project for $ARCH...\e[0m"
  cd "$BUILD_DIR/$ARCH"
  cmake "$ROOT_DIR" -G "Visual Studio 17 2022" -A "$ARCH"

  echo -e "\e[32m\n▶ Building $ARCH in Release...\e[0m"
  cmake --build . --config Release

  echo -e "\e[32m\n▶ Building $ARCH in Debug...\e[0m"
  cmake --build . --config Debug

  echo -e "\e[32m\n▶ Copying built libs for $ARCH...\e[0m"
  cp -v $(find . -type f -name "*.lib" -path "*/Release/*") "$DIST_DIR/Release/$ARCH/" || true
  cp -v $(find . -type f -name "*.lib" -path "*/Debug/*") "$DIST_DIR/Debug/$ARCH/" || true
done

echo -e "\e[32m\n✅ RC2D libraries for Windows x64 + arm64 built successfully!\e[0m"
echo -e "\e[33m→ Output in: $DIST_DIR/\e[0m"
