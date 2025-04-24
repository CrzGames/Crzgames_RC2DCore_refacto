@echo off
setlocal

:: Compatibilité : Ce script est conçu pour fonctionner sur les systèmes Windows.

:: Documentation :
:: Ce script compile les shaders HLSL aux formats SPIR-V (Vulkan) et DXIL (Direct3D12) en utilisant SDL_shadercross.

:: Utilisation :
:: 1. Assurez-vous que SDL_shadercross est installé et disponible dans votre PATH.
:: 2. Exécutez le script :
:: .\scripts\compile_shaders.bat
:: 3. Les shaders compilés seront placés dans le répertoire de sortie spécifié : ..\shaders\compiled
:: 4. Les shaders SPIR-V (Vulkan) seront dans ..\shaders\compiled\spirv
:: 5. Les shaders DXIL (Direct3D12) seront dans ..\shaders\compiled\dxil

:: Vérification de l'existence de SDL_shadercross
if not exist "shadercross.exe" (
    echo SDL_shadercross n'est pas trouvé dans le répertoire courant.
    echo Assurez-vous que SDL_shadercross est installé et disponible dans votre PATH.
    exit /b 1
)

set SHADERCROSS=shadercross.exe
set SRC_DIR=..\shaders\src
set OUT_DIR=..\shaders\compiled

if not exist "%OUT_DIR%\spirv" mkdir "%OUT_DIR%\spirv"
if not exist "%OUT_DIR%\dxil" mkdir "%OUT_DIR%\dxil"

for %%f in (%SRC_DIR%\*.hlsl) do (
    for %%e in (spirv dxil) do (
        %SHADERCROSS% "%%f" -o "%OUT_DIR%\%%e\%%~nf.%%e"
    )
)

echo Ignorer la compilation des shaders MSL sur Windows.
echo Compilation des shaders source HLSL vers SPIR-V (Vulkan) et DXIL (Direct3D12) terminée.
echo Les shaders compilés sont disponibles dans le répertoire de sortie :
echo %OUT_DIR%
echo SPIR-V (Vulkan) : %OUT_DIR%\spirv
echo DXIL (Direct3D12) : %OUT_DIR%\dxil

endlocal
