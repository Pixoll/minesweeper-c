# Minesweeper in C with SDL2

Just a side project to learn more SDL2, and because I like minesweeper.
I'm basing it off of [Minesweeper - The Clean One](https://play.google.com/store/apps/details?id=ee.dustland.android.minesweeper).

Made with the sole purpose of learning.

*Not tested on Linux yet.*

## Dependencies

- CMake `>= 3.28`
- SDL (x64) `>= 2.30.5`
- SDL_image `>= 2.8.2`
- SDL_ttf `>= 2.22.0`

Notes:
- SDL files must be in the directory `C:/SDL2/x64`. If you have SDL installed in a different directory, change the variable `SDL2_PATH` in `CMakeLists.txt` file to your installation directory.
- Make sure to copy all the binaries (`.dll`) into the `CMAKE_CURRENT_BINARY_DIR` directory.

## Compiling

```
cmake --build CMAKE_CURRENT_BINARY_DIR --target minesweeper -j 10
```

## Executing

Just open the `CMAKE_CURRENT_BINARY_DIR/minesweeper.exe` executable.
