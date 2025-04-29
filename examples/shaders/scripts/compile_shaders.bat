@echo off
setlocal

:: Affichage de l'aide si --help est demande
if /i "%~1"=="--help" (
    echo.
    echo ===========================================
    echo Compilation de shaders RC2D
    echo ===========================================
    echo.
    echo Compatibilite : 
    echo     Ce script est concu pour fonctionner sur les systemes Windows.
    echo.
    echo Utilisation :
    echo     compile_shaders.bat [options]
    echo.
    echo Options disponibles :
    echo     --msl-version [version]   Specifie la version de MSL pour Metal
    echo     --only-spirv              Compiler uniquement pour SPIR-V (Vulkan)
    echo     --only-dxil               Compiler uniquement pour DXIL (Direct3D12)
    echo     --only-msl                Compiler uniquement pour MSL (Metal)
    echo     --no-json                 Desactiver la generation des fichiers JSON (reflexion des ressources shaders)
    echo     --help                    Afficher cette aide
    echo.
    echo Comportement par defaut :
    echo     Compile les shaders source HLSL en SPIR-V, DXIL et MSL.
    echo     Genere les fichiers JSON de reflexion des ressources shaders.
    echo     Version MSL par defaut : 2.1
    echo.
    echo Exemples :
    echo     compile_shaders.bat --only-spirv
    echo     compile_shaders.bat --msl-version 2.3 --only-msl
    echo     compile_shaders.bat
    echo.
    echo Documentation :
    echo     Ce script compile les shaders HLSL aux formats SPIR-V (Vulkan), DXIL (Direct3D12), MSL (Metal)
    echo     et JSON (reflexion des ressources shaders) en utilisant le binaire shadercross de SDL3_shadercross.
    echo.
    echo     Les shaders source HLSL doivent etre places dans le repertoire ../src.
    echo     Les shaders compiles seront places dans le repertoire ../compiled.
    echo.
    echo     Repertoires de sortie :
    echo         ../compiled/spirv : shaders SPIR-V (Vulkan)
    echo         ../compiled/msl   : shaders MSL (Metal)
    echo         ../compiled/dxil  : shaders DXIL (Direct3D12)
    echo         ../reflection     : fichiers JSON de reflexion des ressources shaders
    echo.
    echo     Le script verifie si le binaire shadercross est present dans ../tools.
    echo     S'il est absent, un message d'erreur est affiche et le script se termine.
    echo.
    echo     Ensuite, il verifie s'il existe des fichiers HLSL a compiler dans ../src.
    echo     S'il n'y en a pas, le script affiche un message et termine proprement.
    echo     Sinon, il compile tous les fichiers .hlsl trouves.
    echo.
    exit /b 0
)

:: ==================================================
:: Configuration par défaut
:: ==================================================
set MSL_VERSION=2.1
set COMPILE_SPIRV=true
set COMPILE_DXIL=true
set COMPILE_MSL=true
set COMPILE_JSON=true
:: Concernant la version MSL (Metal), il est recommandé de spécifier explicitement le modèle de version cible.
:: SDL_shadercross utilise par défaut MSL 1.2 pour garantir une compatibilité maximale avec d'anciens appareils.
:: Cependant, l'API GPU SDL3 impose des exigences matérielles minimales (macOS 10.14+, iOS/tvOS 13.0+, A9 ou Intel Mac2 GPU minimum),
:: MSL 2.1 ou supérieur systématiquement disponible sur ces appareils.
:: Garder la version 1.2 par défaut n'est donc pas cohérent avec les exigences de SDL3.
:: Il est fortement conseillé d'utiliser --msl-version 2.1 (ou supérieur) pour compiler les shaders Metal destinés à SDL3 GPU API.
set MSL_VERSION=2.1

:: ==================================================
:: Gestion des arguments en ligne de commande
:: ==================================================
:parse_args
if "%~1"=="" goto end_args

if /i "%~1"=="--msl-version" (
    if "%~2"=="" (
        call :print_red "Erreur : --msl-version requiert un argument."
        call :print_red "Usage : compile_shaders.bat --msl-version [version]"
        call :print_red "Exemple : compile_shaders.bat --msl-version 2.3"
        exit /b 1
    )
    set MSL_VERSION=%~2
    shift
) else if /i "%~1"=="--only-spirv" (
    set COMPILE_SPIRV=true
    set COMPILE_DXIL=false
    set COMPILE_MSL=false
) else if /i "%~1"=="--only-dxil" (
    set COMPILE_SPIRV=false
    set COMPILE_DXIL=true
    set COMPILE_MSL=false
) else if /i "%~1"=="--only-msl" (
    set COMPILE_SPIRV=false
    set COMPILE_DXIL=false
    set COMPILE_MSL=true
) else if /i "%~1"=="--no-json" (
    set COMPILE_JSON=false
)

shift
goto parse_args
:end_args

:: ==================================================
:: Variables de chemins
:: ==================================================
set RELATIVE_SHADERCROSS=..\tools\shadercross.exe
set SRC_DIR=..\src
set OUT_COMPILED_DIR=..\compiled
set OUT_REFLECTION_DIR=..\reflection

:: Variable pour le compteur de shaders compilés
set COMPILED_COUNT=0

:: Résoudre le chemin absolu du binaire shadercross
pushd "%~dp0%RELATIVE_SHADERCROSS%\.."
set ABS_SHADERCROSS=%CD%\shadercross.exe
popd

:: Résoudre le chemin absolu du répertoire source des shaders
pushd "%SRC_DIR%"
set ABS_SRC_DIR=%CD%
popd

:: Vérification de l'existence du binaire shadercross local
if not exist "%ABS_SHADERCROSS%" (
    call ::print_red "Le binaire 'shadercross' (SDL3_shadercross) n'est pas trouver a l'emplacement suivant :"
    call ::print_red "%ABS_SHADERCROSS%"
    call ::print_red "Veuillez vous assurer que le binaire et ces dependances sont presents dans le repertoire specifier."
    exit /b 1
)

:: Vérification s'il existe des fichiers .hlsl à compiler dans le répertoire source : %SRC_DIR%
set FOUND_HLSL=false
for %%f in (%SRC_DIR%\*.hlsl) do (
    set FOUND_HLSL=true
    goto :found
)

:: Si aucun fichier .hlsl n'est trouvé, le script affiche un message d'erreur et se termine.
:found
if "%FOUND_HLSL%"=="false" (
    call :print_red "Aucun shader HLSL (.hlsl) trouve dans le repertoire :"
    call :print_red "%ABS_SRC_DIR%"
    exit /b 0
)

:: Création des répertoires de sortie si nécessaire
if not exist "%OUT_COMPILED_DIR%\spirv" mkdir "%OUT_COMPILED_DIR%\spirv"
if not exist "%OUT_COMPILED_DIR%\dxil" mkdir "%OUT_COMPILED_DIR%\dxil"
if not exist "%OUT_COMPILED_DIR%\msl" mkdir "%OUT_COMPILED_DIR%\msl"
if not exist "%OUT_REFLECTION_DIR%" mkdir "%OUT_REFLECTION_DIR%"

:: Compilation des shaders HLSL vers SPIR-V (Vulkan), Metal (MSL), DXIL (Direct3D12) et JSON (réflexion des ressources shaders)
:: Via le binaire SDL_shadercross
for %%f in (%SRC_DIR%\*.hlsl) do (
    set "filename=%%~nf"

    :: Compilation des shaders HLSL vers SPIR-V (Vulkan), DXIL (Direct3D12) et MSL (Metal)
    if "%COMPILE_SPIRV%"=="true" (
        call "%ABS_SHADERCROSS%" "%%f" -o "%OUT_COMPILED_DIR%\spirv\%%~nf.spv"
    )
    if "%COMPILE_DXIL%"=="true" (
        call "%ABS_SHADERCROSS%" "%%f" -o "%OUT_COMPILED_DIR%\dxil\%%~nf.dxil"
    )
    if "%COMPILE_MSL%"=="true" (
        call "%ABS_SHADERCROSS%" "%%f" -o "%OUT_COMPILED_DIR%\msl\%%~nf.msl" --msl-version %MSL_VERSION%
    )

    :: Compilation des fichiers JSON de réflexion des ressources shaders
    if "%COMPILE_JSON%"=="true" (
        call "%ABS_SHADERCROSS%" "%%f" -o "%OUT_REFLECTION_DIR%\%%~nf.json"
    )

    :: Incrémentation du compteur de shaders compilés
    set /A COMPILED_COUNT+=1
)

:: Récupération du répertoire de sortie absolu des shaders compilés
pushd "%OUT_COMPILED_DIR%"
set ABS_OUT_COMPILED_DIR=%CD%
popd

:: Récupération du répertoire de sortie absolu des fichiers JSON de réflexion des ressources shaders
pushd "%OUT_REFLECTION_DIR%"
set ABS_OUT_REFLECTION_DIR=%CD%
popd

:: Affichage des logs pour information
echo.
call :print_success "%COMPILED_COUNT% shader(s) compile(s) avec succes ✅"
echo.

if "%COMPILE_SPIRV%"=="true" (
    echo Compilation des shaders source HLSL vers SPIR-V (Vulkan) terminé.
)
if "%COMPILE_MSL%"=="true" (
    echo Compilation des shaders source HLSL vers Metal (MSL) terminé.
)
if "%COMPILE_DXIL%"=="true" (
    echo Compilation des shaders source HLSL vers DXIL (Direct3D12) terminé.
)
if "%COMPILE_JSON%"=="true" (
    echo Compilation des fichiers JSON de reflexion des ressources shaders terminé.
)

echo.
echo Les shaders compilés sont disponibles dans les répertoires de sortie :
echo.

if "%COMPILE_SPIRV%"=="true" (
    echo SPIR-V (Vulkan) :
    call :print_green "%ABS_OUT_COMPILED_DIR%\spirv"
    echo.
)

if "%COMPILE_MSL%"=="true" (
    echo MSL (Metal) :
    call :print_green "%ABS_OUT_COMPILED_DIR%\msl"
    echo.
)

if "%COMPILE_DXIL%"=="true" (
    echo DXIL (Direct3D12) :
    call :print_green "%ABS_OUT_COMPILED_DIR%\dxil"
    echo.
)

if "%COMPILE_JSON%"=="true" (
    echo JSON (réflexion des ressources shaders) :
    call :print_green "%ABS_OUT_REFLECTION_DIR%"
    echo.
)


endlocal

:: ---------------------------------------
:: Fonctions d'affichage coloré avec préfixes
:: ---------------------------------------
:print_red
    setlocal
    set "TEXT=%~1"
    echo [31m[ERROR] %TEXT%[0m
    endlocal
    goto :eof

:print_green
    setlocal
    set "TEXT=%~1"
    echo [32m[INFO] %TEXT%[0m
    endlocal
    goto :eof

:print_success
    setlocal
    set "TEXT=%~1"
    echo [32m[SUCCESS] %TEXT%[0m
    endlocal
    goto :eof
