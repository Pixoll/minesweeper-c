#include "colors.hpp"

#include <SDL.h>

Color colors[Color::NAMES_AMOUNT];

void init_colors(SDL_Window *window) {
    const SDL_Surface *surface = SDL_GetWindowSurface(window);
    colors[Color::THEME] = {surface, "#d77f37"};
    colors[Color::BACKGROUND] = {surface, "#333333"};

    colors[Color::GRID_1] = {surface, "#b3b3ff"};  // #0000ff
    colors[Color::GRID_2] = {surface, "#b3ffb3"};  // #008000
    colors[Color::GRID_3] = {surface, "#ffb3b3"};  // #ff0000
    colors[Color::GRID_4] = {surface, "#4d4dff"};  // #000080
    colors[Color::GRID_5] = {surface, "#ff4d4d"};  // #800000
    colors[Color::GRID_6] = {surface, "#b3ffff"};  // #008080
    colors[Color::GRID_7] = {surface, "#bfbfbf"};  // #808080
    colors[Color::GRID_8] = {surface, "#ffffff"};

    colors[Color::FLAGGED_CELL] = {surface, "#333333"};
    colors[Color::FLAGGED_CELL_BG] = {surface, "#606060"};
    colors[Color::TRIGGERED_MINE] = {surface, "#431a0d"};
    colors[Color::TRIGGERED_MINE_BG] = {surface, "#b6350d"};

    colors[Color::BLACK] = {surface, "#000000"};
    colors[Color::DARK_GREY] = {surface, "#1e1f1c"};
    colors[Color::GREY] = {surface, "#333333"};
    colors[Color::LIGHT_GREY] = {surface, "#606060"};
    colors[Color::LIGHTER_GREY] = {surface, "#cfcfcf"};
    colors[Color::WHITE] = {surface, "#ffffff"};
}

Color get_color(const Color::Name name) {
    return colors[name];
}
