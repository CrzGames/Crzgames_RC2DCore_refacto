#!/bin/bash

# Compatibilité : Ce script est conçu pour fonctionner sur les systèmes Unix (Linux/macOS).

# Documentation :
# Ce script compile les shaders HLSL aux formats SPIR-V (Vulkan), DXIL (Direct3D12) et MSL (Metal) en utilisant SDL_shadercross.

# Utilisation :
# 1. Assurez-vous que SDL_shadercross est installé et disponible dans votre PATH.
# 2. Rendez le script exécutable :
# chmod +x scripts/compile_shaders.sh
# 3. Exécutez le script :
# ./scripts/compile_shaders.sh
# 4. Les shaders compilés seront placés dans le répertoire de sortie spécifié : ..\shaders\compiled
# 5. Les shaders SPIR-V (Vulkan) seront dans ..\shaders\compiled\spirv
# 6. Les shaders DXIL (Direct3D12) seront dans ..\shaders\compiled\dxil
# 7. Les shaders MSL (Metal) seront dans ..\shaders\compiled\msl

SHADERCROSS="shadercross"
SRC_DIR="../shaders/src"
OUT_DIR="../shaders/compiled"

# Vérification de l'existence du binaire de SDL_shadercross
if ! command -v $SHADERCROSS &> /dev/null; then
    echo "Erreur : SDL_shadercross n'est pas installé ou n'est pas dans le PATH."
    echo "Veuillez installer SDL_shadercross et vous assurer qu'il est accessible."
    exit 1
fi

mkdir -p "$OUT_DIR/spirv" "$OUT_DIR/dxil" "$OUT_DIR/msl"

for file in "$SRC_DIR"/*.hlsl; do
    filename=$(basename "$file" .hlsl)

    $SHADERCROSS "$file" -o "$OUT_DIR/spirv/$filename.spv"
    $SHADERCROSS "$file" -o "$OUT_DIR/dxil/$filename.dxil"
    $SHADERCROSS "$file" -o "$OUT_DIR/msl/$filename.msl"
done

echo Compilation des shaders source HLSL vers SPIR-V (Vulkan), DXIL (Direct3D12) et MSL (Metal) terminée.
echo Les shaders compilés sont disponibles dans le répertoire de sortie :
echo "$OUT_DIR"
echo SPIR-V (Vulkan) : "$OUT_DIR/spirv"
echo DXIL (Direct3D12) : "$OUT_DIR/dxil"
echo MSL (Metal) : "$OUT_DIR/msl"