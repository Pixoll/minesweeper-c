#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "fonts.h"
#include "global.h"
#include "grid.h"
#include "textures.h"
#include "util.h"

#define WINDOW_FLAGS SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE
#define RENDER_FLAGS SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC

const char *windowIconPath = "assets/images/icon.png";

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

    assetArg(columns, 5, 100, "Number of columns must be between 5 and 100");
    assetArg(rows, 5, 100, "Number of rows must be between 5 and 100");
    assetArg(density, 1, 99, "Mines density must be between 1% and 99%");

    const int minesCount = rows * columns * density / 100;
    printf("Grid %dx%d\tMines count: %d (%d%%)\n", columns, rows, minesCount, density);
    createGrid(rows, columns);

    initSDL();
    initColors();

    const SDL_Color bgColor = colors[COLOR_BACKGROUND].rgb;
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);

    SDL_Event event;
    bool quit = false, placedMines = false, clickedMine = false;
    struct timeval lastFrame = {0, 0};

    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_MOUSEBUTTONDOWN: {
                    if (clickedMine) break;
                    int clickX, clickY;
                    SDL_GetMouseState(&clickX, &clickY);
                    const Coords cell = calculateGridCell(clickX, clickY);

                    switch (event.button.button) {
                        case SDL_BUTTON_LEFT: {
                            if (!placedMines) {
                                placeGridMines(minesCount, cell.x, cell.y);
                                placedMines = true;
                            }

                            clickedMine = revealCell(cell.x, cell.y);
                            break;
                        }
                        case SDL_BUTTON_RIGHT: {
                            if (!placedMines) break;
                            toggleCellFlag(cell.x, cell.y);
                            break;
                        }
                        default:
                            break;
                    }
                }
                default:
                    break;
            }
        }

        getWindowSize();
        calculateGridMeasurements();

        initFonts();
        initTextures();

        SDL_RenderClear(renderer);

        drawGrid(clickedMine);

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

    SDL_Surface *icon = IMG_Load(windowIconPath);
    SDL_SetWindowIcon(window, icon);
    SDL_FreeSurface(icon);

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
