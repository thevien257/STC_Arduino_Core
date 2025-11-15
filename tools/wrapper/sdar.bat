@echo off
REM sdar wrapper for Windows

set SDAR=%1
set ARCHIVE=%2
set OBJECT=%3
shift
shift
shift

set ARCHIVE_LIB=%ARCHIVE:.a=.lib%
set OBJECT_REL=%OBJECT:.o=.rel%

if exist %OBJECT% copy /Y %OBJECT% %OBJECT_REL% >nul

%SDAR% %1 %2 %3 %4 %ARCHIVE_LIB% %OBJECT_REL%

if exist %ARCHIVE_LIB% copy /Y %ARCHIVE_LIB% %ARCHIVE% >nul

if exist %OBJECT_REL% del %OBJECT_REL%

exit /b 0