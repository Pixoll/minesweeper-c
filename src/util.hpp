#pragma once

#include <SDL.h>
#include <string>

// General purpose

// #define pxToPt(px) ((px)*0.75)

int random_between(int min, int max);
int is_pow2(int x);
int int_log2(int x);
int int_length(int value);
std::string get_time_string(int seconds);

// SDL related

enum ColorName {
    COLOR_BACKGROUND,
    COLOR_THEME,

    COLOR_GRID_1,
    COLOR_GRID_2,
    COLOR_GRID_3,
    COLOR_GRID_4,
    COLOR_GRID_5,
    COLOR_GRID_6,
    COLOR_GRID_7,
    COLOR_GRID_8,

    COLOR_FLAGGED_CELL,
    COLOR_FLAGGED_CELL_BG,
    COLOR_TRIGGERED_MINE,
    COLOR_TRIGGERED_MINE_BG,

    COLOR_BLACK,
    COLOR_DARK_GREY,
    COLOR_GREY,
    COLOR_LIGHT_GREY,
    COLOR_LIGHTER_GREY,
    COLOR_WHITE,

    COLORS_AMOUNT,
};

struct Color {
    SDL_Color rgb;
    Uint32 value;
};

void init_colors(SDL_Window *window);
Color get_color(ColorName name);

Color map_color(const SDL_Surface *surface, const char *hex_color);
SDL_Surface *create_surface(SDL_Window *window, int width, int height);
SDL_Texture *create_texture(SDL_Renderer *renderer, int width, int height, int access);
SDL_Surface *create_colored_surface(SDL_Window *window, int width, int height, ColorName color);
void throw_sdl_error(const char *function_name, int code = 0);
