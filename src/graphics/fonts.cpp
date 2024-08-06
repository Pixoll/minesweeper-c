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

Font load_font(const Font::Type type, const int size) {
    TTF_Font *font = TTF_OpenFont(font_paths[type], size);
    if (font == nullptr) {
        std::cerr << "Error while loading font ID " << type << " at TTF_OpenFont(): " << SDL_GetError() << std::endl;
        exit(1);
    }

    return {font, size};
}

void init_global_fonts(const int window_height) {
    fonts[Font::PRIMARY] = load_font(Font::RUBIK_REGULAR, window_height * 0.025);
    fonts[Font::SECONDARY] = load_font(Font::RUBIK_REGULAR, window_height * 0.02);
}

void init_main_menu_fonts(const int window_height) {
    fonts[Font::TITLE] = load_font(Font::RUBIK_LIGHT, window_height * 0.04);
}

void init_game_fonts(const int cell_size) {
    fonts[Font::CELL_NUMBER] = load_font(Font::RUBIK_REGULAR, cell_size * 0.5);
}

Font get_font(const Font::Name name) {
    return fonts[name];
}

void free_global_fonts() {
    TTF_CloseFont(fonts[Font::PRIMARY].font);
    TTF_CloseFont(fonts[Font::SECONDARY].font);
}

void free_main_menu_fonts() {
    TTF_CloseFont(fonts[Font::TITLE].font);
}

void free_game_fonts() {
    TTF_CloseFont(fonts[Font::CELL_NUMBER].font);
}
