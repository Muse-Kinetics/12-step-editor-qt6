@echo off
setlocal

if "%~1"=="" exit /b 2
if "%~2"=="" exit /b 2

set "PROJECT_FILE=%~1"
set "ACTION=%~2"
set "ARG3=%~3"
set "ARG4=%~4"
set "BUILD_VARIANT="
set "BUILD_DIR="
set "VCVARS=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
set "QMAKE=C:\qt6\6.3.2\msvc2019_64\bin\qmake.exe"
set "QMAKE_ARGS=%PROJECT_FILE%"
set "POPDIR="

if /i "%ARG3%"=="console" (
    set "BUILD_VARIANT=console"
    set "BUILD_DIR=%ARG4%"
) else (
    set "BUILD_DIR=%ARG3%"
)

if not "%BUILD_DIR%"=="" (
    if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
    pushd "%BUILD_DIR%"
    if errorlevel 1 exit /b %errorlevel%
    set "POPDIR=1"
)

if /i "%BUILD_VARIANT%"=="console" (
    set "QMAKE_ARGS=%PROJECT_FILE% BUILD_CONSOLE=1"
)

call "%VCVARS%"
if errorlevel 1 goto :end

if /i "%ACTION%"=="configure" (
    "%QMAKE%" %QMAKE_ARGS%
    goto :end
)

if /i "%ACTION%"=="build" (
    "%QMAKE%" %QMAKE_ARGS%
    if errorlevel 1 goto :end
    nmake /NOLOGO
    goto :end
)

if /i "%ACTION%"=="clean" (
    nmake /NOLOGO clean
    goto :end
)

echo Unsupported action: %ACTION%
set "ERR=2"
goto :cleanup

:end
set "ERR=%errorlevel%"

:cleanup
if defined POPDIR popd
exit /b %ERR%
