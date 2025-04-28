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
    exit 1
fi

# Création des répertoire source des shaders compilés
mkdir -p "$OUT_DIR/spirv" "$OUT_DIR/dxil" "$OUT_DIR/msl" "$OUT_DIR/json"

# Concernant la version MSL (Metal), il est recommandé de spécifier explicitement le modèle de version cible.
# SDL_shadercross utilise par défaut MSL 1.2 pour garantir une compatibilité maximale avec d'anciens appareils.
# Cependant, l'API GPU SDL3 impose des exigences matérielles minimales (macOS 10.14+, iOS/tvOS 13.0+, A9 ou Intel Mac2 GPU minimum),
# MSL 2.1 ou supérieur systématiquement disponible sur ces appareils.
# Garder la version 1.2 par défaut n'est donc pas cohérent avec les exigences de SDL3.
# Il est fortement conseillé d'utiliser --msl-version 2.1 (ou supérieur) pour compiler les shaders Metal destinés à SDL3 GPU API.
MSL_VERSION="2.1"

# Compilation des shaders HLSL vers SPIR-V (Vulkan), DXIL (Direct3D12), MSL (Metal) et JSON (réflexion des ressources shaders)
# Via le binaire SDL_shadercross
for file in "$SRC_DIR"/*.hlsl; do
    filename=$(basename "$file" .hlsl)

    $SHADERCROSS "$file" -o "$OUT_DIR/spirv/$filename.spv"
    $SHADERCROSS "$file" -o "$OUT_DIR/dxil/$filename.dxil"
    $SHADERCROSS "$file" -o "$OUT_DIR/msl/$filename.msl" --msl-version "$MSL_VERSION"
    $SHADERCROSS "$file" -o "$OUT_DIR/json/$filename.json"
done

# Récupération du répertoire de sortie absolu des shaders compilés
ABS_OUT_DIR="$(cd "$OUT_DIR" && pwd)"

# Affichage des résultats
echo "Compilation des shaders source HLSL vers SPIR-V (Vulkan), DXIL (Direct3D12) et MSL (Metal) terminée."
echo "Les shaders compilés sont disponibles dans le répertoire de sortie :"
echo "$ABS_OUT_DIR"
echo "SPIR-V (Vulkan) : $ABS_OUT_DIR/spirv"
echo "DXIL (Direct3D12) : $ABS_OUT_DIR/dxil"
echo "MSL (Metal) : $ABS_OUT_DIR/msl"
echo "JSON (réflexion des ressources shaders) : $ABS_OUT_DIR/json"