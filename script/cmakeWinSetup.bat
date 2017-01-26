REM Clear the CMAke Cache files
del CMakeCache.txt
del /S /Q CMakeFiles

REM Set Option for cross compilation
SET OPTIONS= -G"MinGW Makefiles"
SET OPTIONS= %OPTIONS% -DCMAKE_AR=C:/SysGCC/Raspberry/bin/arm-linux-gnueabihf-ar.exe
SET OPTIONS= %OPTIONS% -DCMAKE_C_COMPILER=C:/SysGCC/Raspberry/bin/arm-linux-gnueabihf-gcc.exe
SET OPTIONS= %OPTIONS% -DCMAKE_MAKE_PROGRAM=C:/SysGCC/Raspberry/bin/make.exe
SET OPTIONS= %OPTIONS% -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_TOOLCHAIN_FILE="C://Program Files//CMake/share//cmake-3.7//Modules//Platform//Linux-GNU.cmake"
SET OPTIONS= %OPTIONS% -DPERL_EXECUTABLE=C:/Strawberry/perl/bin/perl.exe

REM Disable MBEDtls TEST
SET OPTIONS= %OPTIONS% -DENABLE_TESTING=0 -DENABLE_PROGRAMS=0

REM Statics & dynamic libraries
SET OPTIONS= %OPTIONS% -DUSE_SHARED_MBEDTLS_LIBRARY=0 -DUSE_STATIC_MBEDTLS_LIBRARY=1

echo %OPTIONS%
cmake %OPTIONS%  ..
