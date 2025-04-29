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
# chmod +x ./shaders/scripts/compile_shaders.sh
# 3. Exécutez le script :
# ./shaders/scripts/compile_shaders.sh

RELATIVE_SHADERCROSS="../tools/shadercross"
SRC_DIR="../src"
OUT_COMPILED_DIR="../compiled"
OUT_REFLECTION_DIR="../reflection"

COMPILED_COUNT=0

# ------------------------------------------------
# Fonctions d'affichage coloré avec préfixes
# ------------------------------------------------
print_red() {
    echo -e "\033[31m[ERROR] $1\033[0m"
}

print_green() {
    echo -e "\033[32m[INFO] $1\033[0m"
}

print_success() {
    echo -e "\033[32m[SUCCESS] $1 ✅\033[0m"
}

# Résoudre le chemin absolu du binaire shadercross
ABS_SHADERCROSS="$(cd "$(dirname "$RELATIVE_SHADERCROSS")" && pwd)/$(basename "$RELATIVE_SHADERCROSS")"

# Résoudre le chemin absolu du répertoire source des shaders
ABS_SRC_DIR="$(cd "$(dirname "$SRC_DIR")" && pwd)/$(basename "$SRC_DIR")"

# Vérification de l'existence du binaire shadercross local
if [ ! -f "$ABS_SHADERCROSS" ]; then
    print_red "Le binaire 'shadercross' (SDL3_shadercross) n'est pas trouver à l'emplacement suivant :"
    print_red "$ABS_SHADERCROSS"
    print_red "Veuillez vous assurer que le binaire et ses dependances sont presents dans le repertoire specifier."
    exit 1
fi

# Vérification s'il existe des fichiers .hlsl à compiler
shopt -s nullglob
HLSL_FILES=("$SRC_DIR"/*.hlsl)
shopt -u nullglob

if [ ${#HLSL_FILES[@]} -eq 0 ]; then
    print_red "Aucun shader HLSL (.hlsl) trouvé dans le répertoire suivant :"
    print_red "$ABS_SRC_DIR"
    exit 0
fi

# Création des répertoires de sortie si nécessaire
mkdir -p "$OUT_COMPILED_DIR/spirv" "$OUT_COMPILED_DIR/dxil" "$OUT_COMPILED_DIR/msl" "$OUT_REFLECTION_DIR"

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

    # Compilation des shaders HLSL vers SPIR-V (Vulkan), DXIL (Direct3D12) et MSL (Metal)
    "$ABS_SHADERCROSS" "$file" -o "$OUT_COMPILED_DIR/spirv/$filename.spv"
    "$ABS_SHADERCROSS" "$file" -o "$OUT_COMPILED_DIR/dxil/$filename.dxil"
    "$ABS_SHADERCROSS" "$file" -o "$OUT_COMPILED_DIR/msl/$filename.msl" --msl-version "$MSL_VERSION"

    # Compilation du fichier JSON de réflexion des ressources shaders
    "$ABS_SHADERCROSS" "$file" -o "$OUT_REFLECTION_DIR/$filename.json"

    # Incrémentation du compteur de shaders compilés
    COMPILED_COUNT=$((COMPILED_COUNT + 1))
done

# Récupération du répertoire de sortie absolu des shaders compilés
ABS_COMPILED_DIR="$(cd "$OUT_COMPILED_DIR" && pwd)"

# Récupération du répertoire de sortie absolu des fichiers JSON de réflexion des ressources shaders
ABS_REFLECTION_DIR="$(cd "$OUT_REFLECTION_DIR" && pwd)"

# Affichage des logs pour information
echo ""
print_success "$COMPILED_COUNT shader(s) compile(s) avec succes ✅"
echo ""
echo "Compilation des shaders source HLSL vers SPIR-V (Vulkan), MSL (Metal) et DXIL (Direct3D12) terminer."
echo "Compilation des fichiers JSON de reflexion des ressources shaders terminer."
echo "Les shaders compiler sont disponibles dans les repertoires de sortie :"
echo "SPIR-V (Vulkan) :" 
print_green "$ABS_COMPILED_DIR/spirv"
echo "MSL (Metal) :" 
print_green "$ABS_COMPILED_DIR/msl"
echo "DXIL (Direct3D12) :"
print_green "$ABS_COMPILED_DIR/dxil"
echo "Les fichiers JSON de reflexion des ressources shaders sont disponibles dans le repertoire de sortie :"
print_green "$ABS_REFLECTION_DIR"