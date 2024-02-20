@echo off
REM SetupScript Juergen Riegel mail@juergen-riegel.net
echo  [32mDeploy project.[0m

REM make date-time string

set hour=%time:~0,2%
if "%hour:~0,1%" == " " set hour=0%hour:~1,1%
set min=%time:~3,2%
if "%min:~0,1%" == " " set min=0%min:~1,1%
set secs=%time:~6,2%
if "%secs:~0,1%" == " " set secs=0%secs:~1,1%
set year=%date:~-4%
set month=%date:~3,2%
if "%month:~0,1%" == " " set month=0%month:~1,1%
set day=%date:~0,2%
if "%day:~0,1%" == " " set day=0%day:~1,1%

set BuildTime=%year%%month%%day%-%hour%%min%


echo  [94mCheck for compiler version:[0m

if exist "C:\Program Files\Microsoft Visual Studio\2022\Community" (
  SET MSBUILD_EXE="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional" (
  SET MSBUILD_EXE="C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe"
)else  if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community" (
  SET MSBUILD_EXE="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe"
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional" (
  SET MSBUILD_EXE="C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\MSBuild\Current\Bin\MSBuild.exe"
)else (
  ECHO [91mERROR: compiler whas not found ! [0m
  pause
  exit 1
)

echo "Using: %MSBUILD_EXE%"


%MSBUILD_EXE% /m /nologo /v:m WinBuild\PyOpenJt.sln /p:Configuration=Release -maxcpucount
IF %ERRORLEVEL% NEQ 0 (
    ECHO [91mERROR: Build failed! [0m
    pause
    exit 1
)


rem Remove build dir and rename with build time
rmdir /S / Q PyOpenJt_Dist
timeout 3 > NUL

mkdir PyOpenJt_Dist

robocopy WinBuild/Release PyOpenJt_Dist /s /e



powershell -command "Compress-Archive -Path 'PyOpenJt_Dist' -DestinationPath 'PyOpenJt_Dist.zip'"
IF %ERRORLEVEL% NEQ 0 (
    ECHO [91mERROR: ZIPing failed! [0m
    pause
    exit 1
)

ren PyOpenJt_Dist.zip PyOpenJt_%BuildTime%.zip

rmdir /S / Q PyOpenJt_Dist

