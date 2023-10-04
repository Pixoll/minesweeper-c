@echo off
setlocal EnableDelayedExpansion

set exename=%*
if [!exename!] == [] set exename=minesweeper

set files=
for /f "tokens=*" %%f in ('dir src\*.c /s/b') do (
    set files=!files! "%%f"
)

set command=gcc -fdiagnostics-color=always !files! -I .\include -L .\lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -o "!exename!.exe"
echo Running: %command%
%command%
