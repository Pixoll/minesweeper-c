#pragma once

#include <SDL.h>
#include <string>

#include "global.hpp"

using std::string;

// General purpose

// #define pxToPt(px) ((px)*0.75)

int random_between(int min, int max);
int is_pow2(int x);
int int_log2(int x);
int int_length(int value);
string get_time_string(int seconds);

// SDL related

Color map_color(const SDL_Surface *surface, const char *hex_color);
SDL_Surface *create_surface(int width, int height);
SDL_Texture *create_texture(int width, int height, int access);
SDL_Surface *create_colored_surface(int width, int height, COLOR color);
void throw_sdl_error(const char *function_name, int code = 0);
