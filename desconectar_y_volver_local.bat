@echo off
REM Mismo que conectar_server.bat pero arranca en modo local (sin server)
REM Util para probar el cliente solo en local
setlocal
set "PROJ=%~dp0UniversoIA26.uproject"

set "UE_PATH="
if exist "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe" set "UE_PATH=C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe"
if exist "D:\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe"           set "UE_PATH=D:\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe"
if exist "D:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe" set "UE_PATH=D:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe"
if exist "E:\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe"           set "UE_PATH=E:\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe"

if "%UE_PATH%"=="" (
  echo No encuentro UnrealEditor.exe. Edita el .bat con la ruta correcta.
  pause
  exit /b 1
)

"%UE_PATH%" "%PROJ%" -game -log -windowed -ResX=1280 -ResY=720
