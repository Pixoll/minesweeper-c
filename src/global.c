#include "global.h"

#include <SDL2/SDL.h>

#include "util.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

const int defaultWindowWidth = 1280;
const int defaultWindowHeight = 720;

int windowWidth = -1;
int windowHeight = -1;

Color colors[COLORS_AMOUNT];

void getWindowSize() {
    if (windowHeight != -1 || windowWidth != -1) return;
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    windowHeight = surface->h;
    windowWidth = surface->w;
}

void initColors() {
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    colors[COLOR_THEME] = mapColor(surface, "#d77f37");
    colors[COLOR_BACKGROUND] = mapColor(surface, "#1e1f1c");

    colors[COLOR_GRID_1] = mapColor(surface, "#b3b3ff");  // #0000ff
    colors[COLOR_GRID_2] = mapColor(surface, "#b3ffb3");  // #008000
    colors[COLOR_GRID_3] = mapColor(surface, "#ffb3b3");  // #ff0000
    colors[COLOR_GRID_4] = mapColor(surface, "#4d4dff");  // #000080
    colors[COLOR_GRID_5] = mapColor(surface, "#ff4d4d");  // #800000
    colors[COLOR_GRID_6] = mapColor(surface, "#b3ffff");  // #008080
    colors[COLOR_GRID_7] = mapColor(surface, "#bfbfbf");  // #808080
    colors[COLOR_GRID_8] = mapColor(surface, "#ffffff");

    colors[COLOR_FLAGGED_CELL] = mapColor(surface, "#1e1f1c");
    colors[COLOR_FLAGGED_CELL_BG] = mapColor(surface, "#606060");
    colors[COLOR_TRIGGERED_MINE] = mapColor(surface, "#431a0d");
    colors[COLOR_TRIGGERED_MINE_BG] = mapColor(surface, "#b6350d");

    colors[COLOR_BLACK] = mapColor(surface, "#000000");
    colors[COLOR_DARK_GREY] = mapColor(surface, "#1e1f1c");
    colors[COLOR_GREY] = mapColor(surface, "#333333");
    colors[COLOR_LIGHT_GREY] = mapColor(surface, "#606060");
    colors[COLOR_WHITE] = mapColor(surface, "#ffffff");
}
