#include "fonts.hpp"

#include <cstdlib>
#include <iostream>
#include <SDL_ttf.h>

const char *font_paths[Font::TYPES] = {
    "assets/fonts/Rubik-Light.ttf",
    "assets/fonts/Rubik-Medium.ttf",
    "assets/fonts/Rubik-Regular.ttf",
    "assets/fonts/Rubik-Bold.ttf",
};

Font fonts[Font::NAMES];

void throw_sdl_error(const char *function_name, int code = 0);

Font load_font(const Font::Type type, const int size) {
    TTF_Font *font = TTF_OpenFont(font_paths[type], size);
    if (font == nullptr) {
        std::cerr << "Error while loading font ID " << type << " at TTF_OpenFont(): " << SDL_GetError() << std::endl;
        exit(1);
    }

    return {font, size};
}

void init_global_fonts(const int window_height) {
    fonts[Font::RUBIK_MEDIUM_PRIMARY] = load_font(Font::RUBIK_MEDIUM, window_height * 0.02);
    fonts[Font::RUBIK_MEDIUM_SECONDARY] = load_font(Font::RUBIK_MEDIUM, window_height * 0.01875);
}

void init_game_fonts(const int cell_size) {
    fonts[Font::RUBIK_MEDIUM_CELL_SIZED] = load_font(Font::RUBIK_MEDIUM, cell_size * 0.5);
}

Font get_font(const Font::Name name) {
    return fonts[name];
}

void free_fonts() {
    for (auto &[font, size] : fonts) {
        TTF_CloseFont(font);
        font = nullptr;
    }
}
