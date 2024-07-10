@echo off

set preset="ALL"
if not "%1" == "" (
    set preset=%1
)

echo Running preset %preset%

cmake -S . --preset=%preset%
if %ErrorLevel% NEQ 0 (
    goto end
)

cmake --build build --config Release
if %ErrorLevel% NEQ 0 (
    goto end
)

:end
pause
