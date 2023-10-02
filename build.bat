@echo off
setlocal EnableDelayedExpansion

set exename=%*
if [!exename!] == [] set exename=minesweeper

set files=
for /f "tokens=*" %%f in ('dir src\*.c /s/b') do (
    set files=!files! "%%f"
)

set command=gcc !files! -I .\include -L .\lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -o "!exename!.exe"
echo Running: %command%
%command%
