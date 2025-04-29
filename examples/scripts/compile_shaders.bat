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
:: .\scripts\compile_shaders.bat

set SHADERCROSS=shadercross
set SRC_DIR=..\shaders\src
set OUT_DIR=..\shaders\compiled

:: Vérification de l'existence du binaire SDL_shadercross
where %SHADERCROSS% >nul 2>nul
if errorlevel 1 (
    echo Erreur : SDL_shadercross n'est pas installé ou n'est pas dans le PATH.
    exit /b 1
)

:: Création des répertoire source des shaders compilés
if not exist "%OUT_DIR%\spirv" mkdir "%OUT_DIR%\spirv"
if not exist "%OUT_DIR%\dxil" mkdir "%OUT_DIR%\dxil"
if not exist "%OUT_DIR%\json" mkdir "%OUT_DIR%\json"

:: Compilation des shaders HLSL vers SPIR-V (Vulkan), DXIL (Direct3D12) et JSON (réflexion des ressources shaders)
:: Via le binaire SDL_shadercross
for %%f in (%SRC_DIR%\*.hlsl) do (
    set "filename=%%~nf"

    call %SHADERCROSS% "%%f" -o "%OUT_DIR%\spirv\%%~nf.spv"
    call %SHADERCROSS% "%%f" -o "%OUT_DIR%\dxil\%%~nf.dxil"
    call %SHADERCROSS% "%%f" -o "%OUT_DIR%\json\%%~nf.json"
)

:: Récupération du répertoire de sortie absolu des shaders compilés
pushd "%OUT_DIR%"
set ABS_OUT_DIR=%CD%
popd

:: Affichage des logs pour information
echo Ignore la compilation des shaders MSL sur Windows.
echo Compilation des shaders source HLSL vers SPIR-V (Vulkan) et DXIL (Direct3D12) terminée.
echo Compilation des fichiers JSON de réflexion des ressources shaders terminée.
echo Les shaders compilés sont disponibles dans le répertoire de sortie :
echo %ABS_OUT_DIR%
echo SPIR-V (Vulkan) : %ABS_OUT_DIR%\spirv
echo DXIL (Direct3D12) : %ABS_OUT_DIR%\dxil
echo JSON (réflexion des ressources shaders) : %ABS_OUT_DIR%\json

endlocal
