@echo off
REM SetupScript Juergen Riegel mail@juergen-riegel.net
echo  [32mSetup the build environment.[0m


echo  [94mSearch for VCPKG:[0m
WHERE vcpkg
IF %ERRORLEVEL% NEQ 0 (
    ECHO [91mERROR: vcpkg.exe whas not found in path! Download and bootstrap vcpkg first! [0m
    ECHO See: https://github.com/microsoft/vcpkg on details for you system.
    pause
    exit 1
)
FOR /F "tokens=* USEBACKQ" %%F IN (`where.exe vcpkg.exe`) DO (
 SET VCPKG_DIR=%%~dpF
 GoTo Found
)
:Found
echo Using VCPKG in %VCPKG_DIR%

echo  [94mSearch for cMake:[0m
WHERE cmake
IF %ERRORLEVEL% NEQ 0 (
    ECHO [91mERROR: cmake.exe whas not found in path! Download and install CMAKE first! [0m
    ECHO See: https://cmake.org/ on details for you system.
    pause
    exit 1
)

echo  [94mInstall dependencies:[0m

call %VCPKG_DIR%vcpkg.exe install ^
   qt5-base:x64-windows ^
   qt5-tools:x64-windows ^
   eigen3:x64-windows^
   opencascade:x64-windows ^
   tbb:x64-windows ^
   cli11:x64-windows ^
   tinygltf:x64-windows ^
   liblzma:x64-windows ^
   zlib:x64-windows ^
   tinygltf ^
   draco
   

  

echo  [94mCheck for compiler version:[0m
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community" (
  SET GENERATOR_STRING="Visual Studio 17 2022"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional" (
  SET GENERATOR_STRING="Visual Studio 17 2022"
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community" (
  SET GENERATOR_STRING="Visual Studio 16 2019"
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional" (
  SET GENERATOR_STRING="Visual Studio 16 2019"
) else (
  ECHO [91mERROR: compiler whas not found ! [0m
  pause
  exit 1
)
echo Using cMake generator: %GENERATOR_STRING%

cmake.exe -G %GENERATOR_STRING% -B WinBuild -S . -A x64 -DCMAKE_TOOLCHAIN_FILE=%VCPKG_DIR%scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows 


