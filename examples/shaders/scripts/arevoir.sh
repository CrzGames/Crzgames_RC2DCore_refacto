#!/bin/bash
set -e

# ===============================================
# Configuration par défaut
# ===============================================
MSL_VERSION="3.0.0"
COMPILE_SPIRV=true
COMPILE_DXIL=true
COMPILE_MSL=true
COMPILE_JSON=true

HAS_ONLY_OPTION=false

# ===============================================
# Gestion des arguments en ligne de commande
# ===============================================
print_help() {
    cat <<EOF

===========================================
RC2D - Compilation de shaders hors ligne
===========================================

Utilisation :
    ./compile_shaders.sh [options]

Options :
    --msl-version [version]  Version MSL pour Metal (ex: 3.0.0)
    --only-spirv             Compile uniquement SPIR-V
    --only-dxil              Compile uniquement DXIL
    --only-msl               Compile uniquement MSL
    --no-json                Désactive génération JSON
    --help                   Affiche cette aide

Exemples :
    ./compile_shaders.sh --only-dxil
    ./compile_shaders.sh --only-msl --msl-version 2.3.0 --no-json
EOF
    exit 0
}

while [[ "$#" -gt 0 ]]; do
    case $1 in
        --msl-version)
            MSL_VERSION="$2"
            shift 2;;
        --only-spirv) COMPILE_SPIRV=true; COMPILE_DXIL=false; COMPILE_MSL=false; HAS_ONLY_OPTION=true; shift ;;
        --only-dxil) COMPILE_DXIL=true; COMPILE_SPIRV=false; COMPILE_MSL=false; HAS_ONLY_OPTION=true; shift ;;
        --only-msl) COMPILE_MSL=true; COMPILE_SPIRV=false; COMPILE_DXIL=false; HAS_ONLY_OPTION=true; shift ;;
        --no-json) COMPILE_JSON=false; shift ;;
        --help) print_help ;;
        *) echo "[Erreur] Option inconnue : $1"; exit 1 ;;
    esac
done

# ===============================================
# Variables des chemins
# ===============================================
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
SHADERCROSS="$SCRIPT_DIR/../tools/shadercross"
SRC_DIR="$SCRIPT_DIR/../src"
OUT_COMPILED_DIR="$SCRIPT_DIR/../compiled"
OUT_REFLECTION_DIR="$SCRIPT_DIR/../reflection"

# Vérifications préliminaires
if [ ! -x "$SHADERCROSS" ]; then
    echo "[Erreur] shadercross introuvable ou non exécutable : $SHADERCROSS"
    exit 1
fi

if ! ls "$SRC_DIR"/*.hlsl &>/dev/null; then
    echo "[Erreur] Aucun shader HLSL trouvé dans $SRC_DIR"
    exit 0
fi

# Création des dossiers
mkdir -p "$OUT_COMPILED_DIR"/{spirv,dxil,msl,metallib}
[ "$COMPILE_JSON" = true ] && mkdir -p "$OUT_REFLECTION_DIR"

# ===============================================
# Compilation des shaders
# ===============================================
COMPILED_COUNT=0
SKIPPED_COUNT=0
TOTAL_COUNT=0

for shader in "$SRC_DIR"/*.hlsl; do
    TOTAL_COUNT=$((TOTAL_COUNT+1))
    shader_name=$(basename "$shader" .hlsl)

    if ! grep -q "main" "$shader"; then
        echo "[SKIP] $shader_name.hlsl (pas de point d'entrée main)"
        SKIPPED_COUNT=$((SKIPPED_COUNT+1))
        continue
    fi

    echo "[COMPILE] $shader_name.hlsl"

    # SPIR-V
    if [ "$COMPILE_SPIRV" = true ]; then
        "$SHADERCROSS" "$shader" -o "$OUT_COMPILED_DIR/spirv/$shader_name.spv"
    fi

    # DXIL
    if [ "$COMPILE_DXIL" = true ]; then
        "$SHADERCROSS" "$shader" -o "$OUT_COMPILED_DIR/dxil/$shader_name.dxil"
    fi

    # MSL
    if [ "$COMPILE_MSL" = true ]; then
        "$SHADERCROSS" "$shader" -o "$OUT_COMPILED_DIR/msl/$shader_name.msl" --msl-version "$MSL_VERSION"

        # Compilation MSL → MetalLib (macOS uniquement)
        if command -v xcrun >/dev/null 2>&1; then
            xcrun metal -c "$OUT_COMPILED_DIR/msl/$shader_name.msl" -o "$OUT_COMPILED_DIR/metallib/$shader_name.air"
            xcrun metallib "$OUT_COMPILED_DIR/metallib/$shader_name.air" -o "$OUT_COMPILED_DIR/metallib/$shader_name.metallib"
            rm "$OUT_COMPILED_DIR/metallib/$shader_name.air"
            echo "[MetalLib OK] $shader_name.metallib"
        else
            echo "[⚠️] Commande xcrun introuvable, impossible de compiler MSL → MetalLib (uniquement macOS)."
        fi
    fi

    # JSON Reflection
    if [ "$COMPILE_JSON" = true ]; then
        "$SHADERCROSS" "$shader" -o "$OUT_REFLECTION_DIR/$shader_name.json"
    fi

    COMPILED_COUNT=$((COMPILED_COUNT+1))
done

# ===============================================
# Affichage du résumé
# ===============================================
echo ""
echo "Compilation terminée :"
echo "Total traités : $TOTAL_COUNT"
echo "Shaders compilés : $COMPILED_COUNT"
echo "Shaders ignorés : $SKIPPED_COUNT"

echo ""
[ "$COMPILE_SPIRV" = true ] && echo "SPIR-V: $OUT_COMPILED_DIR/spirv"
[ "$COMPILE_DXIL" = true ] && echo "DXIL: $OUT_COMPILED_DIR/dxil"
[ "$COMPILE_MSL" = true ] && echo "MSL: $OUT_COMPILED_DIR/msl"
[ "$COMPILE_MSL" = true ] && echo "MetalLib: $OUT_COMPILED_DIR/metallib"
[ "$COMPILE_JSON" = true ] && echo "JSON Reflection: $OUT_REFLECTION_DIR"
