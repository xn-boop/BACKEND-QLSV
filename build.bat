@echo off
title Student Management System - Build
color 0A

echo.
echo  ============================================
echo    Student Management System  v3.0
echo    Building with g++ (C++17)
echo  ============================================
echo.

g++ src/main.cpp src/Student.cpp src/StudentManager.cpp -I include -std=c++17 -Wall -o sms.exe

if %errorlevel%==0 (
    echo.
    echo  [OK] BUILD SUCCESSFUL!
    echo  ============================================
    echo.
    echo  Starting program...
    echo.
    sms.exe
) else (
    echo.
    echo  [ERROR] BUILD FAILED.
    echo  Make sure g++ is installed:
    echo    winget install mingw
    echo  or download from: https://www.mingw-w64.org/
    echo.
    pause
)
