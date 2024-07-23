#include "util.hpp"

#include <cstdlib>
#include <iostream>
#include <SDL.h>
#include <string>
#include <unistd.h>

#include "global.hpp"

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

SDL_Surface *create_surface(const int width, const int height) {
    const Uint32 pixel_format = SDL_GetWindowSurface(window)->format->format;
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, pixel_format);
    return surface;
}

SDL_Texture *create_texture(const int width, const int height, const int access) {
    const Uint32 pixel_format = SDL_GetWindowSurface(window)->format->format;
    SDL_Texture *texture = SDL_CreateTexture(renderer, pixel_format, access, width, height);
    return texture;
}

SDL_Surface *create_colored_surface(const int width, const int height, const COLOR color) {
    SDL_Surface *surface = create_surface(width, height);
    const SDL_Rect area = {0, 0, width, height};
    const Uint32 surface_color = colors[color].value;
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
