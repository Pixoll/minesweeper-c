#pragma once

#include <SDL_ttf.h>

struct Font {
    TTF_Font *font;
    int size;

    enum Type {
        RUBIK_LIGHT,
        RUBIK_MEDIUM,
        RUBIK_REGULAR,
        RUBIK_BOLD,
    };

    static constexpr int TYPES = RUBIK_BOLD + 1;

    enum Name {
        RUBIK_MEDIUM_CELL_SIZED,
        RUBIK_MEDIUM_PRIMARY,
        RUBIK_MEDIUM_SECONDARY,
    };

    static constexpr int NAMES = RUBIK_MEDIUM_SECONDARY + 1;
};

void init_global_fonts(int window_height);
void init_game_fonts(int cell_size);
Font get_font(Font::Name name);
void free_fonts();
