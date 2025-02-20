@echo off
git submodule update --init --recursive
premake\premake5 --file=premake\sln_structure.lua vs2022
pause