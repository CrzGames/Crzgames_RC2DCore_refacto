#!/bin/bash

# Compatibilité : 
# Ce script est conçu pour fonctionner sur les systèmes Unix (Linux/macOS).

# Documentation :
# Ce script compile les shaders HLSL aux formats SPIR-V (Vulkan), DXIL (Direct3D12), MSL (Metal) et JSON (réflexion des ressources shaders).
# Il utilise le binaire SDL_shadercross pour effectuer la compilation.
# Les shaders source HLSL doivent être placés dans le répertoire ../shaders/src.
# Les shaders compilés seront placés dans le répertoire ../shaders/compiled.
# Les répertoires de sortie pour les shaders compilés sont organisés comme suit :
# - ../shaders/compiled/spirv : pour les shaders SPIR-V (Vulkan)
# - ../shaders/compiled/dxil : pour les shaders DXIL (Direct3D12)
# - ../shaders/compiled/msl : pour les shaders MSL (Metal)
# - ../shaders/compiled/json : pour les fichiers JSON de réflexion des ressources shaders
# Le script vérifie d'abord si le binaire SDL_shadercross est installé et disponible dans le PATH.
# Si le binaire n'est pas trouvé, le script affiche un message d'erreur et se termine.
# Ensuite, il crée les répertoires de sortie nécessaires pour les shaders compilés.
# Il compile chaque fichier HLSL dans le répertoire source en utilisant SDL_shadercross et génère les fichiers de sortie correspondants.
# Le script utilise la version 2.1 de MSL (Metal) par défaut, mais vous pouvez modifier la variable MSL_VERSION pour spécifier une autre version si nécessaire.
# Ce script est conçu pour être exécuté dans le répertoire racine du projet.

# Utilisation :
# 1. Assurez-vous que SDL_shadercross est installé et disponible dans votre PATH.
# 2. Rendez le script exécutable :
# chmod +x scripts/compile_shaders.sh
# 3. Exécutez le script :
# ./scripts/compile_shaders.sh

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
# A voir pour la version 2.3 : iOS 14.0+ iPadOS 14.0+ macOS 11.0+ tvOS 14.0+

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

# Affichage des logs pour information
echo "Compilation des shaders source HLSL vers SPIR-V (Vulkan), DXIL (Direct3D12) et MSL (Metal) terminée."
echo "Compilation des fichiers JSON de réflexion des ressources shaders terminée."
echo "Les shaders compilés sont disponibles dans le répertoire de sortie :"
echo "$ABS_OUT_DIR"
echo "SPIR-V (Vulkan) : $ABS_OUT_DIR/spirv"
echo "DXIL (Direct3D12) : $ABS_OUT_DIR/dxil"
echo "MSL (Metal) : $ABS_OUT_DIR/msl"
echo "JSON (réflexion des ressources shaders) : $ABS_OUT_DIR/json"