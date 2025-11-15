@echo off
REM SDCC Linker wrapper for Windows

set SDCC=%1
shift

set FLAGS=
set REL_FILES=
set DRIVER_FILES=

:parse_args
if "%1"=="" goto done_parsing
set ARG=%1

REM Check for .o files
echo %ARG% | findstr /i ".o$" >nul
if %errorlevel%==0 (
    set REL_FILE=%ARG:.o=.rel%
    if not exist !REL_FILE! copy %ARG% !REL_FILE! >nul
    
    echo %ARG% | findstr /i "drivers\\src" >nul
    if %errorlevel%==0 (
        set DRIVER_FILES=!DRIVER_FILES! !REL_FILE!
    ) else (
        set REL_FILES=!REL_FILES! !REL_FILE!
    )
    shift
    goto parse_args
)

REM Check for .a files
echo %ARG% | findstr /i ".a$" >nul
if %errorlevel%==0 (
    set LIB_FILE=%ARG:.a=.lib%
    if not exist !LIB_FILE! copy %ARG% !LIB_FILE! >nul
    set REL_FILES=!REL_FILES! !LIB_FILE!
    shift
    goto parse_args
)

REM Check for output flag
if "%ARG%"=="-o" (
    shift
    set FLAGS=%FLAGS% -o %1
    shift
    goto parse_args
)

REM Other flags
set FLAGS=%FLAGS% %ARG%
shift
goto parse_args

:done_parsing

REM For now, link with all drivers (simple approach for Windows)
%SDCC% %FLAGS% %REL_FILES% %DRIVER_FILES%

exit /b %errorlevel%