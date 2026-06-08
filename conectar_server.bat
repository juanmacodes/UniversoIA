@echo off
REM Lanza el cliente UniversoIA26 conectandose al server AWS/OVH
REM Si tienes UE5.7 en otra ruta, cambia UE_PATH abajo

setlocal
set "PROJ=%~dp0UniversoIA26.uproject"
set "SERVER=51.95.38.254:7777"

REM Rutas habituales donde se instala el Unreal Editor
set "UE_PATH="
if exist "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe" set "UE_PATH=C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe"
if exist "D:\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe"           set "UE_PATH=D:\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe"
if exist "D:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe" set "UE_PATH=D:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe"
if exist "E:\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe"           set "UE_PATH=E:\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe"

if "%UE_PATH%"=="" (
  echo No encuentro UnrealEditor.exe en las rutas habituales.
  echo Edita este .bat y ajusta la variable UE_PATH a la ruta de tu instalacion de UE5.7
  echo Ejemplo: set "UE_PATH=D:\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe"
  pause
  exit /b 1
)

echo Editor:  %UE_PATH%
echo Proyecto: %PROJ%
echo Server:  %SERVER%
echo.
echo Lanzando cliente...
"%UE_PATH%" "%PROJ%" %SERVER% -game -log -windowed -ResX=1280 -ResY=720
