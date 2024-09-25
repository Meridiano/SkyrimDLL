@echo off

cd /d "%~dp0"

git init
git submodule add -b ng https://github.com/alandtse/CommonLibVR.git lib/commonlibsse-ng
git submodule update --init --remote --recursive

pause
