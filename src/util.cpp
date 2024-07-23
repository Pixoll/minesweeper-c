#include "util.hpp"

#include <cstdlib>
#include <iostream>
#include <SDL.h>
#include <string>
#include <unistd.h>

Color colors[COLORS_AMOUNT];

int random_between(const int min, const int max) {
    return rand() % (max - min + 1) + min;
}

int is_pow2(const int x) {
    return x > 0 && !(x & (x - 1));
}

int int_log2(int x) {
    int log2 = 0;
    while (x >>= 1)
        log2++;

    return log2;
}

int int_length(int value) {
    int length = 1;
    while (value > 9) {
        length++;
        value /= 10;
    }
    return length;
}

std::string get_time_string(const int seconds) {
    using std::string, std::to_string;

    const int sec = seconds % 60;
    const int min = seconds / 60;

    string time_string;

    if (min == 0)
        time_string += to_string(sec) + "S";
    else
        time_string += to_string(min) + "M " + to_string(sec) + "S";

    return time_string;
}

void init_colors(SDL_Window *window) {
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

Color get_color(const ColorName name) {
    return colors[name];
}

Color map_color(const SDL_Surface *surface, const char *hex_color) {
    if (hex_color[0] == '#')
        hex_color++;  // shift left once

    const int rgb = strtol(hex_color, nullptr, 16);
    const Uint8 r = rgb >> 16 & 0xff;
    const Uint8 g = rgb >> 8 & 0xff;
    const Uint8 b = rgb & 0xff;

    Color color;
    color.rgb = {r, g, b, 255};
    color.value = SDL_MapRGB(surface->format, r, g, b);
    return color;
}

SDL_Surface *create_surface(SDL_Window *window, const int width, const int height) {
    const Uint32 pixel_format = SDL_GetWindowSurface(window)->format->format;
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, pixel_format);
    return surface;
}

SDL_Texture *create_texture(SDL_Renderer *renderer, const int width, const int height, const int access) {
    const Uint32 pixel_format = SDL_GetWindowSurface(SDL_RenderGetWindow(renderer))->format->format;
    SDL_Texture *texture = SDL_CreateTexture(renderer, pixel_format, access, width, height);
    return texture;
}

SDL_Surface *create_colored_surface(SDL_Window *window, const int width, const int height, const ColorName color) {
    SDL_Surface *surface = create_surface(window, width, height);
    const SDL_Rect area = {0, 0, width, height};
    const Uint32 surface_color = get_color(color).value;
    SDL_FillRect(surface, &area, surface_color);
    return surface;
}

void throw_sdl_error(const char *function_name, const int code) {
    using std::cerr, std::endl;

    cerr << "Error ";

    if (code != 0)
        cerr << code << " ";

    cerr << "at " << function_name << "(): " << SDL_GetError() << endl;
    exit(1);
}
