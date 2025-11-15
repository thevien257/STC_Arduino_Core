@echo off
REM packihx wrapper for Windows

set PACKIHX=%1
set INPUT=%2
set OUTPUT=%3

%PACKIHX% %INPUT% > %OUTPUT% 2>nul

exit /b %errorlevel%