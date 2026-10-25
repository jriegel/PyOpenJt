@echo off
REM Build Release, create the Python wheel, and install it into .venv
setlocal
cd /d "%~dp0"

echo === Release build ===
cmake --build WinBuild --config Release
if errorlevel 1 goto :fail

echo === Build wheel ===
if defined CMAKE_TOOLCHAIN_FILE goto :havetc
if not defined VCPKG_ROOT for /f "delims=" %%F in ('where vcpkg.exe') do if not defined VCPKG_ROOT set "VCPKG_ROOT=%%~dpF"
if not defined VCPKG_ROOT (
    echo ERROR: vcpkg not found; set VCPKG_ROOT.
    goto :fail
)
set "CMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake"
:havetc
echo Toolchain: %CMAKE_TOOLCHAIN_FILE%
set "VCPKG_DEFAULT_TRIPLET=x64-windows"
rmdir /S /Q build\wheel 2>nul
if exist dist\repaired rmdir /S /Q dist\repaired
if exist dist\raw rmdir /S /Q dist\raw
python -m pip install --quiet --upgrade build delvewheel scikit-build-core
if errorlevel 1 goto :fail
python -m pip wheel . --no-deps --no-build-isolation -w dist\raw
if errorlevel 1 goto :fail
for %%W in (dist\raw\pyopenjt-*.whl) do (
    python -m delvewheel repair -w dist\repaired "%%W" --add-path "build\wheel\vcpkg_installed\x64-windows\bin;build\wheel\Release"
    if errorlevel 1 goto :fail
)

echo === Python venv ===
if not exist .venv\Scripts\python.exe (
    echo Creating .venv
    python -m venv .venv
    if errorlevel 1 goto :fail
) else (
    echo .venv already present
)
for %%W in (dist\repaired\pyopenjt-*.whl) do (
    .venv\Scripts\python.exe -m pip install --force-reinstall --no-deps "%%W"
    if errorlevel 1 goto :fail
)
.venv\Scripts\python.exe -c "import PyOpenJt; print('PyOpenJt OK', PyOpenJt.__file__)"
if errorlevel 1 goto :fail

echo === Done ===
endlocal
exit /b 0

:fail
echo ERROR: BuildPy failed.
endlocal
exit /b 1
