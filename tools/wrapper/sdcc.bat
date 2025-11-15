@echo off
REM SDCC wrapper for Windows

set SDCC=%1
set SRC=%2
set OBJ=%3
set MARK=%4
shift
shift
shift
shift

REM Convert .o to .rel
set REL=%OBJ:.o=.rel%

REM Compile based on file extension
echo %SRC% | findstr /i ".cpp$" >nul
if %errorlevel%==0 (
    REM Compile C++ as C
    %SDCC% %1 %2 %3 %4 %5 %6 %7 %8 %9 -x c --include dummy_variable_main.h %SRC% -o %OBJ%
) else (
    REM Compile C file
    %SDCC% %1 %2 %3 %4 %5 %6 %7 %8 %9 %SRC% -o %OBJ%
)

set ERR=%errorlevel%

REM Copy .o to .rel
if exist %OBJ% copy /Y %OBJ% %REL% >nul

exit /b %ERR%