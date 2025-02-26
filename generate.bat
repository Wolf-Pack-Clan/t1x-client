@echo off
git submodule update --init --recursive
premake\premake5 --file=premake\premake5.lua vs2022
pause