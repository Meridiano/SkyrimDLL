@echo off

cd /d "%~dp0"

git init
git submodule add -b main https://github.com/CharmedBaryon/CommonLibSSE-NG.git lib/commonlibsse-ng
git submodule update --init --remote --recursive

pause
