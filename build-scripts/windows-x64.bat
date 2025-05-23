@echo off
setlocal enabledelayedexpansion

rem Enable ANSI escape codes
for /f "tokens=2 delims=:" %%i in ('"prompt $H & for %%b in (1) do rem"') do set "BS=%%i"

rem Couleurs ANSI
set "GREEN=\e[32m"
set "RESET=\e[0m"

rem Function: print in green
set "ESC=[27m"
call :print_green "Generating Visual Studio 2022 project for Windows x64..."

cmake -S . -B build\windows\x64 ^
  -G "Visual Studio 17 2022" ^
  -A x64 ^
  -DRC2D_ARCH=x64

for %%b in (Debug Release) do (
  call :print_green "Building %%b..."
  cmake --build build\windows\x64 --config %%b
)

call :print_green "Lib RC2D for Windows x64 built successfully for Debug and Release."
echo Go to dist\lib\windows\x64.

goto :eof

:print_green
echo [[32m%~1[0m]
goto :eof
