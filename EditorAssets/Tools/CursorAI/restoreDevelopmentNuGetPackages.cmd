@echo off
setlocal
rem DevelopmentBuild.yml の Restore NuGet packages 相当（packages.config → Project\packages）

set "REPO_ROOT=%~1"
if "%REPO_ROOT%"=="" (
  echo Usage: restoreDevelopmentNuGetPackages.cmd REPO_ROOT
  exit /b 1
)

set "PACKAGES_CONFIG=%REPO_ROOT%\packages.config"
set "PACKAGES_DIR=%REPO_ROOT%\Project\packages"
set "SDL_TARGETS=%PACKAGES_DIR%\sdl2.redist.2.0.5\build\native\sdl2.redist.targets"

if not exist "%PACKAGES_CONFIG%" (
  echo packages.config not found: %PACKAGES_CONFIG%
  exit /b 1
)

set "NUGET="
where nuget >nul 2>&1
if %errorlevel%==0 (
  set "NUGET=nuget"
) else if exist "%ProgramFiles(x86)%\NuGet\nuget.exe" (
  set "NUGET=%ProgramFiles(x86)%\NuGet\nuget.exe"
)

if not defined NUGET (
  echo nuget.exe not found. Install NuGet CLI or use Visual Studio Developer Command Prompt.
  exit /b 1
)

"%NUGET%" restore "%PACKAGES_CONFIG%" -PackagesDirectory "%PACKAGES_DIR%" -NonInteractive
if errorlevel 1 exit /b 1

if not exist "%SDL_TARGETS%" (
  echo SDL2 targets missing after restore: %SDL_TARGETS%
  exit /b 1
)

echo NuGet restore for Development: OK
exit /b 0