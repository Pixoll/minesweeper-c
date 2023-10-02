#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "fonts.h"
#include "global.h"
#include "grid.h"
#include "textures.h"
#include "util.h"

#define WINDOW_FLAGS SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE
#define RENDER_FLAGS SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC

void initSDL();
void closeSDL();
void exceptArg(char *args[], int n, const char *message);
void assetArg(int arg, int min, int max, const char *message);

int main(int argc, char *argv[]) {
    exceptArg(argv, 1, "Provide the number of columns");
    exceptArg(argv, 2, "Provide the number of rows");

    const int columns = atoi(argv[1]);
    const int rows = atoi(argv[2]);
    const int density = argv[3] ? atoi(argv[3]) : 20;

    assetArg(columns, 5, 100, "Number of columns must be between 5 and 10");
    assetArg(rows, 5, 100, "Number of rows must be between 5 and 10");
    assetArg(density, 1, 99, "Number of columns must be between 1% and 99%");

    const int minesCount = rows * columns * density / 100;
    printf("Grid %dx%d\tMines count: %d (%d%%)\n", columns, rows, minesCount, density);
    createGrid(rows, columns, minesCount);

    initSDL();
    initColors();

    SDL_Event event;
    bool quit = false;

    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                default:
                    break;
            }
        }

        getWindowSize();
        calculateGridMeasurements(rows, columns);

        initFonts();
        initTextures();

        SDL_RenderClear(renderer);

        drawGrid(rows, columns);

        SDL_RenderPresent(renderer);
    }

    freeTextures();
    freeFonts();
    closeSDL();
    return 0;
}

void initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) throwSDLError("SDL_Init");

    window = SDL_CreateWindow("Minesweeper", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              defaultWindowWidth, defaultWindowHeight, WINDOW_FLAGS);
    if (window == NULL) throwSDLError("SDL_CreateWindow");

    renderer = SDL_CreateRenderer(window, -1, RENDER_FLAGS);
    if (renderer == NULL) throwSDLError("SDL_CreateRenderer");

    const int ttfReady = TTF_Init();
    if (ttfReady != 0) throwSDLError("TTF_Init");
}

void closeSDL() {
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void exceptArg(char *args[], int n, const char *message) {
    if (args[n]) return;
    printf("arg%d: %s\n", n, message);
    exit(1);
}

void assetArg(int arg, int min, int max, const char *message) {
    if (arg >= min && arg <= max) return;
    printf("%s\n", message);
    exit(1);
}
