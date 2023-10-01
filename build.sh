#!/bin/bash

exename=$1
if [[ -z $exename ]]; then
    exename=minesweeper
fi

files=""
for file in `find ./src -type f -name "*.c"`; do
    files="$files \"$file\"" 
done

command="gcc -g $files -o $exename.exe -lSDL2 -lSDL2_ttf"
echo Running: $command
eval $command
