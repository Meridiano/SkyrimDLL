@echo off

git init
git submodule add -b ng https://github.com/alandtse/CommonLibVR.git external/CommonLibSSE-NG
git submodule update --init --remote --recursive

pause
