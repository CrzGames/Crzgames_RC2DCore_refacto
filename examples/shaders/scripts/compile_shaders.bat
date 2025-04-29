@echo off
setlocal

:: Compatibilité : 
:: Ce script est conçu pour fonctionner sur les systèmes Windows.

:: Documentation :
:: Ce script compile les shaders HLSL aux formats SPIR-V (Vulkan), DXIL (Direct3D12) et JSON (réflexion des ressources shaders).
:: Il utilise le binaire SDL_shadercross pour effectuer la compilation.
:: Les shaders source HLSL doivent être placés dans le répertoire ../shaders/src.
:: Les shaders compilés seront placés dans le répertoire ../shaders/compiled.
:: Les répertoires de sortie pour les shaders compilés sont organisés comme suit :
:: - ../shaders/compiled/spirv : pour les shaders SPIR-V (Vulkan)
:: - ../shaders/compiled/dxil : pour les shaders DXIL (Direct3D12)
:: - ../shaders/compiled/json : pour les fichiers JSON de réflexion des ressources shaders
:: Le script vérifie d'abord si le binaire SDL_shadercross est installé et disponible dans le PATH.
:: Si le binaire n'est pas trouvé, le script affiche un message d'erreur et se termine.
:: Ensuite, il crée les répertoires de sortie nécessaires pour les shaders compilés.
:: Il compile chaque fichier HLSL dans le répertoire source en utilisant SDL_shadercross et génère les fichiers de sortie correspondants.
:: Ce script est conçu pour être exécuté dans le répertoire racine du projet.

:: Utilisation :
:: 1. Assurez-vous que SDL_shadercross est installé et disponible dans votre PATH.
:: 2. Exécutez le script :
:: .\shaders\scripts\compile_shaders.bat

set RELATIVE_SHADERCROSS=..\tools\shadercross.exe
set SRC_DIR=..\src
set OUT_COMPILED_DIR=..\compiled
set OUT_REFLECTION_DIR=..\reflection

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

:: Compilation des shaders HLSL vers SPIR-V (Vulkan), DXIL (Direct3D12) et JSON (réflexion des ressources shaders)
:: Via le binaire SDL_shadercross
for %%f in (%SRC_DIR%\*.hlsl) do (
    set "filename=%%~nf"

    :: Compilation des shaders HLSL vers SPIR-V (Vulkan), DXIL (Direct3D12) et MSL (Metal)
    call "%ABS_SHADERCROSS%" "%%f" -o "%OUT_COMPILED_DIR%\spirv\%%~nf.spv"
    call "%ABS_SHADERCROSS%" "%%f" -o "%OUT_COMPILED_DIR%\dxil\%%~nf.dxil"
    call "%ABS_SHADERCROSS%" "%%f" -o "%OUT_COMPILED_DIR%\msl\%%~nf.msl"

    :: Compilation des fichiers JSON de réflexion des ressources shaders
    call "%ABS_SHADERCROSS%" "%%f" -o "%OUT_REFLECTION_DIR%\%%~nf.json"

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
echo Ignore la compilation des shaders MSL sur Windows.
echo Compilation des shaders source HLSL vers SPIR-V (Vulkan) et DXIL (Direct3D12) terminer.
echo Compilation des fichiers JSON de reflexion des ressources shaders terminer.
echo Les shaders compiler sont disponibles dans les repertoires de sortie :
echo SPIR-V (Vulkan) : 
call ::print_green "%ABS_OUT_COMPILED_DIR%\spirv"
echo MSL (Metal) :
call ::print_green "%ABS_OUT_COMPILED_DIR%\msl"
echo DXIL (Direct3D12) : 
call ::print_green "%ABS_OUT_COMPILED_DIR%\dxil"
echo Les fichiers JSON de reflexion des ressources shaders sont disponibles dans le repertoire de sortie :
call ::print_green "%ABS_OUT_REFLECTION_DIR%"

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
