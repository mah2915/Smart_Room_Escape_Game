@echo off
setlocal
where g++ >nul 2>nul
if errorlevel 1 (
 echo Install a C++17 MinGW-w64 compiler and add its bin directory to PATH.
 exit /b 1
)
pushd "%~dp0"
if not exist build mkdir build
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src\Game.cpp src\main.cpp -o build\smart_escape_room.exe
if errorlevel 1 (popd & exit /b 1)
echo Built build\smart_escape_room.exe
popd
