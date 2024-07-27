#pragma once

#include <SDL_ttf.h>

struct Font {
    TTF_Font *font;
    int size;
};

enum FontType {
    FONT_RUBIK_LIGHT,
    FONT_RUBIK_MEDIUM,
    FONT_RUBIK_REGULAR,
    FONT_RUBIK_BOLD,
};

constexpr int FONT_TYPES = FONT_RUBIK_BOLD + 1;

enum FontName {
    FONT_RUBIK_MEDIUM_CELL_SIZED,
    FONT_RUBIK_MEDIUM_PRIMARY,
    FONT_RUBIK_MEDIUM_SECONDARY,
};

constexpr int FONT_NAMES = FONT_RUBIK_MEDIUM_SECONDARY + 1;

void init_fonts(int window_height, int cell_size);
Font get_font(FontName name);
void free_fonts();
