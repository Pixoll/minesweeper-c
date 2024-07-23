#include "global.hpp"

#include <SDL.h>

#include "util.hpp"

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

const int default_window_width = 1280;
const int default_window_height = 720;

int window_width = -1;
int window_height = -1;

Color colors[COLORS_AMOUNT];

void get_window_size() {
    if (window_height != -1 || window_width != -1)
        return;

    SDL_GetWindowSize(window, &window_width, &window_height);
}

void init_colors() {
    const SDL_Surface *surface = SDL_GetWindowSurface(window);
    colors[COLOR_THEME] = map_color(surface, "#d77f37");
    colors[COLOR_BACKGROUND] = map_color(surface, "#333333");

    colors[COLOR_GRID_1] = map_color(surface, "#b3b3ff");  // #0000ff
    colors[COLOR_GRID_2] = map_color(surface, "#b3ffb3");  // #008000
    colors[COLOR_GRID_3] = map_color(surface, "#ffb3b3");  // #ff0000
    colors[COLOR_GRID_4] = map_color(surface, "#4d4dff");  // #000080
    colors[COLOR_GRID_5] = map_color(surface, "#ff4d4d");  // #800000
    colors[COLOR_GRID_6] = map_color(surface, "#b3ffff");  // #008080
    colors[COLOR_GRID_7] = map_color(surface, "#bfbfbf");  // #808080
    colors[COLOR_GRID_8] = map_color(surface, "#ffffff");

    colors[COLOR_FLAGGED_CELL] = map_color(surface, "#333333");
    colors[COLOR_FLAGGED_CELL_BG] = map_color(surface, "#606060");
    colors[COLOR_TRIGGERED_MINE] = map_color(surface, "#431a0d");
    colors[COLOR_TRIGGERED_MINE_BG] = map_color(surface, "#b6350d");

    colors[COLOR_BLACK] = map_color(surface, "#000000");
    colors[COLOR_DARK_GREY] = map_color(surface, "#1e1f1c");
    colors[COLOR_GREY] = map_color(surface, "#333333");
    colors[COLOR_LIGHT_GREY] = map_color(surface, "#606060");
    colors[COLOR_LIGHTER_GREY] = map_color(surface, "#cfcfcf");
    colors[COLOR_WHITE] = map_color(surface, "#ffffff");
}
