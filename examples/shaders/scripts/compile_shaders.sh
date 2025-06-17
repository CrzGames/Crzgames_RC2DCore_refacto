#!/bin/bash

# ==================================================
# Fonctions d'affichage coloré
# ==================================================
print_red()       { echo -e "\033[31m[ERROR] $1\033[0m"; }
print_green()     { echo -e "\033[93m[INFO]  $1\033[0m"; }
print_success()   { echo -e "\033[32m[SUCCESS] $1\033[0m"; }
print_cyan()      { echo -e "\033[36m[PATH]  $1\033[0m"; }

# Vérifie si un fichier shader contient une fonction main
check_shader_main() {
    grep -q "main" "$1"
}

# Affiche un résumé de la compilation
print_summary() {
    echo -e "\033[93m[SUMMARY] Compilation\033[0m"
    echo -e "\033[37m  Total shader source .hlsl traité : $TOTAL_HLSL_COUNT\033[0m"
    echo -e "\033[33m  Total shader source .hlsl ignoré : $SKIPPED_COUNT\033[0m"
    echo -e "\033[32m  Total shader compilé avec succès : $COMPILED_COUNT\033[0m"
    if [ "$COMPILE_JSON" = true ]; then
        echo -e "\033[32m  Total fichiers JSON généré : $COMPILED_COUNT\033[0m"
    fi
}

# ==================================================
# Fonction pour afficher l'aide détaillée
# ==================================================
print_help() {
    echo
    echo "==========================================="
    echo "RC2D - Compilation de shaders hors ligne"
    echo "==========================================="
    echo
    echo "Compatibilité :"
    echo "    Ce script est conçu pour fonctionner sur les systèmes UNIX (macOS/Linux)."
    echo
    echo "Utilisation :"
    echo "    compile_shaders.sh [options]"
    echo
    echo "Options disponibles :"
    echo "    --msl-version [version]   Spécifie la version de MSL / METALLIB pour Metal"
    echo "    --only-spirv              Compiler uniquement pour SPIR-V (Vulkan)"
    echo "    --only-dxil               Compiler uniquement pour DXIL (Direct3D12)"
    echo "    --only-msl                Compiler uniquement pour MSL et METALLIB (Metal)"
    echo "    --only-pssl               Compiler uniquement pour PSSL (PlayStation Shading Language)"
    echo "    --no-json                 Désactiver la génération des fichiers JSON (réflexion des ressources shaders)"
    echo "    --help                    Afficher cette aide"
    echo
    echo "Comportement par défaut :"
    echo "    Compile les shaders source HLSL en : SPIR-V (Vulkan), DXIL (Direct3D12), MSL / METALLIB (Metal), et PSSL (PlayStation Shading Language)."
    echo "    Génère les fichiers JSON : Les informations de réflexion automatique sur les ressources utilisées par un shader."
    echo "    Version MSL / METALLIB par défaut : 3.2.0 (macOS 15.0+, iOS/iPadOS 18.0+)."
    echo
    echo "Exemples :"
    echo "    compile_shaders.sh --only-dxil"
    echo "    compile_shaders.sh --only-msl --msl-version 2.3.0 --no-json"
    echo "    compile_shaders.sh --only-spirv --only-msl"
    echo "    compile_shaders.sh --only-pssl"
    echo "    compile_shaders.sh"
    echo
    echo "Requis :"
    echo "    SDL3_shadercross CLI (binaire shadercross) doit être présent dans le répertoire : ../tools."
    echo
    echo "Documentation :"
    echo "    Ce script compile les shaders HLSL aux formats SPIR-V (Vulkan), DXIL (Direct3D12), MSL / METALLIB (Metal) et PSSL (PlayStation Shading Language)."
    echo "    Les fichiers JSON de réflexion des ressources shaders sont générés pour chaque shader source HLSL."
    echo
    echo "    Les shaders source HLSL doivent être placés dans le répertoire ../src."
    echo "    Les shaders compilés seront placés dans le répertoire ../compiled."
    echo "    Les fichiers JSON de réflexion des ressources shaders seront placés dans le répertoire ../reflection."
    echo
    echo "    Répertoires de sortie :"
    echo "        ../compiled/spirv    : shaders SPIR-V (Vulkan)"
    echo "        ../compiled/msl      : shaders MSL (Metal)"
    echo "        ../compiled/metallib : shaders METALLIB (Metal)"
    echo "        ../compiled/dxil     : shaders DXIL (Direct3D12)"
    echo "        ../compiled/pssl     : shaders PSSL (PlayStation Shading Language)"
    echo "        ../reflection        : fichiers JSON de réflexion des ressources shaders"
    echo
    echo "    Le script vérifie si le binaire shadercross est présent dans ../tools."
    echo "    S'il est absent, un message d'erreur est affiché et le script se termine."
    echo
    echo "    Ensuite, il vérifie s'il existe des fichiers HLSL à compiler dans ../src."
    echo "    S'il n'y en a pas, le script affiche un message et termine proprement."
    echo "    Sinon, il compile tous les fichiers .hlsl trouvés."
    echo
}

# ==================================================
# Configuration par défaut
# ==================================================
# Documentation version MSL : https://developer.apple.com/documentation/metal/mtllanguageversion
MSL_VERSION="3.2.0"
COMPILE_SPIRV=false
COMPILE_DXIL=false
COMPILE_MSL=false
COMPILE_PSSL=false
COMPILE_JSON=true
HAS_ONLY_OPTION=false

# ==================================================
# Gestion des arguments en ligne de commande
# ==================================================
parse_args() {
    while [ $# -gt 0 ]; do
        case "$1" in
            --help)
                print_help
                exit 0
                ;;
            --msl-version)
                if [ -z "$2" ]; then
                    print_red "--msl-version requiert un argument."
                    print_red "Usage : compile_shaders.sh --msl-version [version]"
                    print_red "Exemple : compile_shaders.sh --msl-version 2.3.0"
                    exit 1
                fi
                MSL_VERSION="$2"
                shift 2
                ;;
            --only-spirv)
                COMPILE_SPIRV=true
                HAS_ONLY_OPTION=true
                shift
                ;;
            --only-dxil)
                COMPILE_DXIL=true
                HAS_ONLY_OPTION=true
                shift
                ;;
            --only-msl)
                COMPILE_MSL=true
                HAS_ONLY_OPTION=true
                shift
                ;;
            --only-pssl)
                COMPILE_PSSL=true
                HAS_ONLY_OPTION=true
                shift
                ;;
            --no-json)
                COMPILE_JSON=false
                shift
                ;;
            *)
                print_red "Argument inconnu : $1"
                print_red "Utilisez --help pour afficher la liste des options disponibles."
                exit 1
                ;;
        esac
    done
}

# Appeler la fonction de parsing des arguments
parse_args "$@"

# Si une option --only-* a été utilisée, désactiver les formats non spécifiés
if [ "$HAS_ONLY_OPTION" = true ]; then
    [ "$COMPILE_SPIRV" != true ] && COMPILE_SPIRV=false
    [ "$COMPILE_DXIL" != true ] && COMPILE_DXIL=false
    [ "$COMPILE_MSL" != true ] && COMPILE_MSL=false
    [ "$COMPILE_PSSL" != true ] && COMPILE_PSSL=false
else
    # Si aucune option --only-* n'a été utilisée, activer tous les formats par défaut
    COMPILE_SPIRV=true
    COMPILE_DXIL=true
    COMPILE_MSL=true
    COMPILE_PSSL=true
fi

# ==================================================
# Variables de chemins
# ==================================================
RELATIVE_SHADERCROSS="../tools/shadercross"
SRC_DIR="../src"
OUT_COMPILED_DIR="../compiled"
OUT_REFLECTION_DIR="../reflection"

# Variable pour le compteur de shaders compilés
COMPILED_COUNT=0
TOTAL_HLSL_COUNT=0
SKIPPED_COUNT=0

# Résoudre le chemin absolu du binaire shadercross
ABS_SHADERCROSS="$(cd "$(dirname "$0")/../tools" && pwd)/shadercross"

# Résoudre le chemin absolu du répertoire source des shaders
ABS_SRC_DIR=$(cd "$SRC_DIR" && pwd)

# Vérification de l'existence du binaire shadercross local
if [ ! -f "$ABS_SHADERCROSS" ]; then
    print_red "Le binaire 'shadercross' (SDL3_shadercross) n'est pas trouvé à l'emplacement suivant :"
    print_red "$ABS_SHADERCROSS"
    print_red "Veuillez vous assurer que le binaire et ses dépendances sont présents dans le répertoire spécifié."
    exit 1
fi

# Vérification s'il existe des fichiers .hlsl à compiler dans le répertoire source
FOUND_HLSL=false
for f in "$SRC_DIR"/*.hlsl; do
    if [ -f "$f" ]; then
        FOUND_HLSL=true
        break
    fi
done

# Si aucun fichier .hlsl n'est trouvé, le script affiche un message d'erreur et se termine
if [ "$FOUND_HLSL" = false ]; then
    print_red "Aucun shader source HLSL (.hlsl) trouvé dans le répertoire :"
    print_red "$ABS_SRC_DIR"
    exit 0
fi

# Création des répertoires de sortie si nécessaire
if [ "$COMPILE_SPIRV" = true ]; then
    mkdir -p "$OUT_COMPILED_DIR/spirv"
fi
if [ "$COMPILE_DXIL" = true ]; then
    mkdir -p "$OUT_COMPILED_DIR/dxil"
fi
if [ "$COMPILE_MSL" = true ]; then
    mkdir -p "$OUT_COMPILED_DIR/msl"

    # Création des dossiers metallib/ios et metallib/macos si on est sur macOS
    if [[ "$OSTYPE" == "darwin"* ]]; then
        mkdir -p "$OUT_COMPILED_DIR/metallib/macos"
        mkdir -p "$OUT_COMPILED_DIR/metallib/ios"
    fi
fi
if [ "$COMPILE_PSSL" = true ]; then
    mkdir -p "$OUT_COMPILED_DIR/pssl"
fi
if [ "$COMPILE_JSON" = true ]; then
    mkdir -p "$OUT_REFLECTION_DIR"
fi

# Compilation des shaders HLSL vers SPIR-V (Vulkan), Metal (MSL), DXIL (Direct3D12) et JSON (réflexion des ressources shaders)
# Via le binaire SDL_shadercross
for f in "$SRC_DIR"/*.hlsl; do
    if [ -f "$f" ]; then
        ((TOTAL_HLSL_COUNT++))
        filename=$(basename "$f" .hlsl)

        # Vérifier que le fichier contient une fonction main en tant que point d'entrée dans le shader
        # Si le fichier ne contient pas de point d'entrée "main", l'ignorer et passer au suivant
        if ! check_shader_main "$f"; then
            echo -e "\033[33m[SKIP] Ignore le shader source $(basename "$f") : aucun point d'entrée \"main\"\033[0m"
            ((SKIPPED_COUNT++))
        else
            # Compilation des shaders HLSL vers SPIR-V (Vulkan)
            if [ "$COMPILE_SPIRV" = true ]; then
                "$ABS_SHADERCROSS" "$f" -o "$OUT_COMPILED_DIR/spirv/$filename.spv"
            fi

            # Compilation des shaders HLSL vers DXIL (Direct3D12)
            if [ "$COMPILE_DXIL" = true ]; then
                "$ABS_SHADERCROSS" "$f" -o "$OUT_COMPILED_DIR/dxil/$filename.dxil"
            fi

            # Compilation des shaders HLSL vers MSL (Metal)
            if [ "$COMPILE_MSL" = true ]; then
                "$ABS_SHADERCROSS" "$f" -o "$OUT_COMPILED_DIR/msl/$filename.msl" --msl-version "$MSL_VERSION"

                # Compilation .msl → .metallib (macOS et iOS)
                if [[ "$OSTYPE" == "darwin"* ]]; then
                    MSL_INPUT="$OUT_COMPILED_DIR/msl/$filename.msl"
                    cp "$MSL_INPUT" tmp.metal

                    # Compilation macOS → .air
                    xcrun -sdk macosx metal -std=metal3.2 -mmacosx-version-min=15.0 -Wall -O3 \
                        -c tmp.metal -o tmp_macos.air 2> tmp_macos.log

                    if grep -q "error:" tmp_macos.log; then
                        print_red "[Metal macOS] Compilation échouée pour $filename.msl"
                        cat tmp_macos.log
                    fi

                    xcrun -sdk macosx metallib tmp_macos.air -o "$OUT_COMPILED_DIR/metallib/macos/$filename.metallib" 2>> tmp_macos.log
                    rm -f tmp_macos.air tmp_macos.log

                    # Compilation iOS → .air
                    xcrun -sdk iphoneos metal -std=metal3.2 -miphoneos-version-min=18.0 -Wall -O3 \
                        -c tmp.metal -o tmp_ios.air 2> tmp_ios.log

                    if grep -q "error:" tmp_ios.log; then
                        print_red "[Metal iOS] Compilation échouée pour $filename.msl"
                        cat tmp_ios.log
                    fi

                    xcrun -sdk iphoneos metallib tmp_ios.air -o "$OUT_COMPILED_DIR/metallib/ios/$filename.metallib" 2>> tmp_ios.log
                    rm -f tmp_ios.air tmp_ios.log

                    rm -f tmp.metal
                fi
            fi

            # Compilation des shaders HLSL vers PSSL (PlayStation Shading Language)
            if [ "$COMPILE_PSSL" = true ]; then
                "$ABS_SHADERCROSS" "$f" -o "$OUT_COMPILED_DIR/pssl/$filename.hlsl" --pssl
            fi

            # Compilation des fichiers JSON de réflexion des ressources shaders
            if [ "$COMPILE_JSON" = true ]; then
                "$ABS_SHADERCROSS" "$f" -o "$OUT_REFLECTION_DIR/$filename.json"
            fi

            # Incrémentation du compteur de shaders compilés
            ((COMPILED_COUNT++))
        fi
    fi
done

# Récupération du répertoire de sortie absolu des shaders compilés
ABS_OUT_COMPILED_DIR=$(cd "$OUT_COMPILED_DIR" && pwd)

# Récupération du répertoire de sortie absolu des fichiers JSON de réflexion des ressources shaders
ABS_OUT_REFLECTION_DIR=$(cd "$OUT_REFLECTION_DIR" && pwd)

echo
print_summary
echo

if [ "$COMPILE_SPIRV" = true ]; then
    print_green "SPIR-V (Shaders Vulkan) :"
    print_cyan "$ABS_OUT_COMPILED_DIR/spirv"
fi
if [ "$COMPILE_MSL" = true ]; then
    print_green "MSL (Shaders Metal) :"
    print_cyan "$ABS_OUT_COMPILED_DIR/msl"

    if [[ "$OSTYPE" == "darwin"* ]]; then
        print_green "METALLIB (macOS - Shaders Metal) :"
        print_cyan "$ABS_OUT_COMPILED_DIR/metallib/macos"

        print_green "METALLIB (iOS - Shaders Metal) :"
        print_cyan "$ABS_OUT_COMPILED_DIR/metallib/ios"
    fi
fi
if [ "$COMPILE_DXIL" = true ]; then
    print_green "DXIL (Shaders Direct3D12) :"
    print_cyan "$ABS_OUT_COMPILED_DIR/dxil"
fi
if [ "$COMPILE_PSSL" = true ]; then
    print_green "PSSL (Shaders PlayStation Shading Language) :"
    print_cyan "$ABS_OUT_COMPILED_DIR/pssl"
fi
if [ "$COMPILE_JSON" = true ]; then
    print_green "JSON (Informations de réflexion sur les ressources shaders) :"
    print_cyan "$ABS_OUT_REFLECTION_DIR"
fi

exit 0