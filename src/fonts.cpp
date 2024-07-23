#include "fonts.hpp"

#include <cstdio>
#include <SDL_ttf.h>

#include "global.hpp"
#include "grid.hpp"
#include "util.hpp"

const char *font_paths[FONT_TYPES] = {
    "assets/fonts/Rubik-Light.ttf",
    "assets/fonts/Rubik-Medium.ttf",
    "assets/fonts/Rubik-Regular.ttf",
    "assets/fonts/Rubik-Bold.ttf",
};

Font font_rubik_medium_cell_sized;
Font font_rubik_medium_primary;
Font font_rubik_medium_secondary;

bool fonts_ready = false;

Font load_font(const FONT_TYPE type, const int size) {
    TTF_Font *font = TTF_OpenFont(font_paths[type], size);
    if (font == nullptr) {
        printf("Error while loading font ID %d\n", type);
        throw_sdl_error("TTF_OpenFont");
    }
    return Font{font, size};
}

void init_fonts() {
    if (fonts_ready)
        return;

    font_rubik_medium_cell_sized = load_font(FONT_RUBIK_MEDIUM, game.measurements.cell_size * 0.5);
    font_rubik_medium_primary = load_font(FONT_RUBIK_MEDIUM, window_height * 0.02);
    font_rubik_medium_secondary = load_font(FONT_RUBIK_MEDIUM, window_height * 0.01875);

    fonts_ready = true;
}

void free_fonts() {
    TTF_CloseFont(font_rubik_medium_cell_sized.font);
    TTF_CloseFont(font_rubik_medium_primary.font);
    TTF_CloseFont(font_rubik_medium_secondary.font);
}
