:: ##############################################################################################
:: app specific variables - adjust these to match your needs and the script should do the rest.
:: Do not use quotes in these paths.
:: ##############################################################################################

set app_name=12 Step Editor
set app_version=2.1.1

:: NO/YES
set USE_QML=NO

set path_to_project_file=%CD%\..\QT\12StepEditor.pro
set path_to_project_folder=%CD%\..\QT
set build_dir=%CD%\..\build-12StepEditor-Desktop_Qt_6_3_2_MSVC2019_64bit-Release

set binary=12 Step Editor.exe
set debug_binary=12 Step Editor (debug console).exe

set path_to_changelog="%CD%\..\CHANGELOG.md"

set path_to_ssl=.\ssl
set path_to_qt_root=C:\Qt6
set path_to_qt=C:\Qt6\6.3.2

:: use quotes in these paths
set path_to_vcvarsall="c:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
set path_to_signtool="C:\Program Files (x86)\Windows Kits\10\bin\10.0.22000.0\x64\signtool.exe"

:: ##############################################################################################
:: end user defined variables, begin inferred variables.
:: ##############################################################################################

set package_dir="%CD%\%app_name%"
set binary_dest="%CD%\%app_name%\%binary%"
set output_zip="%CD%\%app_name% WIN v%app_version%"

set binary_src="%build_dir%\release\%binary%"
set debug_binary_src="%build_dir%\release\%debug_binary%"

set path_to_qmake="%path_to_qt%\msvc2019_64\bin\qmake.exe"
set path_to_jom="%path_to_qt_root%\Tools\QtCreator\bin\jom\jom.exe"
set path_to_qtwindeploy="%path_to_qt%\msvc2019_64\bin\windeployqt.exe"
set path_to_qtsetenv="%path_to_qt%\msvc2019_64\bin\qtenv2.bat"

IF %USE_QML%=="YES" (
	set path_to_qml_files=%path_to_project_folder%
	set deploy_opts=--force --qmldir %path_to_qml_files%
	set qmake_qml_cmd="CONFIG+=qtquickcompiler"
) ELSE (
	set deploy_opts=--force
	set qmake_qml_cmd=
)

:: ##############################################################################################
:: end compound variables
:: ##############################################################################################


::@echo off

set "current_dir=%cd%"

cls


@echo ##############################################################################################
@echo %app_name% package builder
@echo:
@echo Please pause dropbox sync before continuing (avoids zip errors)
@echo ##############################################################################################
@echo: 

pause
cls

@echo ##############################################################################################
@echo Setting up environment for Qt usage...
@echo ##############################################################################################
@echo:

set PATH=%path_to_qt%\msvc2019_64\bin;%PATH%

call %path_to_qtsetenv%
call %path_to_vcvarsall% x64

pause
cls

@echo ##############################################################################################
@echo Running qmake in build dir: %build_dir%
@echo ##############################################################################################
@echo:

IF NOT EXIST "%build_dir%" (
    mkdir "%build_dir%"
)

cd %build_dir%

call :RunCommand qmake "%path_to_project_file%" -spec win32-msvc "DEBUG_CONSOLE=1" %qmake_qml_cmd%

@echo ##############################################################################################
@echo Running jom: %path_to_jom%
@echo ##############################################################################################
@echo:

call :RunCommand %path_to_jom% qmake_all
call :RunCommand %path_to_jom%

@echo ##############################################################################################
@echo Cleaning folder: %package_dir% 
@echo ##############################################################################################
@echo:

cd %current_dir%

IF NOT EXIST %package_dir% (
    echo creating %package_dir%
    mkdir %package_dir%
) else (
	call :RunCommand del /Q %package_dir%\*.*	
)


@echo ##############################################################################################
@echo Copying Changelog...
@echo ##############################################################################################
@echo:

call :RunCommand xcopy /Y %path_to_changelog% %package_dir%


@echo ##############################################################################################
@echo Copying application executables...
@echo ##############################################################################################
@echo:

call :RunCommand xcopy /Y %binary_src% %package_dir%
::xcopy /Y %debug_binary_src% %package_dir%

@echo ##############################################################################################
@echo Running qtwindeploy...
@echo ##############################################################################################
@echo:

call :RunCommand %path_to_qtwindeploy% --release --verbose 2 %deploy_opts% --dir %package_dir%\ %binary_dest%


@echo ##############################################################################################
@echo Running qtwindeploy dry run
@echo ##############################################################################################
@echo:

call :RunCommand %path_to_qtwindeploy% --dry-run %binary_dest%

@echo ##############################################################################################
@echo Copying OpenSSL DLLs
@echo ##############################################################################################
@echo:

:: contents of the ssl folder need to be present in the same dir as the executable
call :RunCommand xcopy /Y %path_to_ssl%\*.* %package_dir%

@echo ##############################################################################################
@echo Signing executables...
@echo ##############################################################################################
@echo:

call :RunCommand %path_to_signtool% sign /debug /a /tr http://timestamp.globalsign.com/tsa/advanced /td SHA256 /fd certHash %binary_dest%


@echo ##############################################################################################
@echo "Wait 5 seconds for dropbox to catch up"
@echo ##############################################################################################
@echo:

TIMEOUT 5

@echo ##############################################################################################
@echo "Creating zip package..."
@echo ##############################################################################################
@echo:

call :RunCommand powershell compress-archive "'%package_dir%' '%output_zip%.zip'" 

@echo ##############################################################################################
@echo "Finished!"
@echo ##############################################################################################
@echo:

pause

exit /b 0

:: END OF SCRIPT, SUBROUTINES BELOW

goto :eof

:: Subroutine to run a command and check if it was successful
:RunCommand
    %*
    IF %ERRORLEVEL% NEQ 0 (
        echo Warning - COMMAND FAILED: '%*'
        pause
        cls
    ) ELSE (
    	echo Success! - '%*'
    	pause
    	cls
    )
goto :eof

