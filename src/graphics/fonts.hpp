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
        PRIMARY,
        SECONDARY,
        TITLE,
        CELL_NUMBER,
    };

    static constexpr int NAMES = CELL_NUMBER + 1;
};

void init_global_fonts(int window_height);
void init_main_menu_fonts(int window_height);
void init_game_fonts(int cell_size);

Font get_font(Font::Name name);

void free_global_fonts();
void free_main_menu_fonts();
void free_game_fonts();
