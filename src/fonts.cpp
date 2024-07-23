#include "fonts.hpp"

#include <iostream>
#include <SDL_ttf.h>

#include "grid.hpp"
#include "util.hpp"

const char *font_paths[FONT_TYPES] = {
    "assets/fonts/Rubik-Light.ttf",
    "assets/fonts/Rubik-Medium.ttf",
    "assets/fonts/Rubik-Regular.ttf",
    "assets/fonts/Rubik-Bold.ttf",
};

Font fonts[FONT_NAMES];

bool fonts_ready = false;

Font load_font(const FontType type, const int size) {
    TTF_Font *font = TTF_OpenFont(font_paths[type], size);
    if (font == nullptr) {
        std::cerr << "Error while loading font ID " << type << std::endl;
        throw_sdl_error("TTF_OpenFont");
    }

    return {font, size};
}

void init_fonts(const int window_height) {
    if (fonts_ready)
        return;

    fonts[FONT_RUBIK_MEDIUM_CELL_SIZED] = load_font(FONT_RUBIK_MEDIUM, game.measurements.cell_size * 0.5);
    fonts[FONT_RUBIK_MEDIUM_PRIMARY] = load_font(FONT_RUBIK_MEDIUM, window_height * 0.02);
    fonts[FONT_RUBIK_MEDIUM_SECONDARY] = load_font(FONT_RUBIK_MEDIUM, window_height * 0.01875);

    fonts_ready = true;
}

Font get_font(const FontName name) {
    return fonts[name];
}

void free_fonts() {
    for (const auto &[font, size] : fonts)
        TTF_CloseFont(font);
}
