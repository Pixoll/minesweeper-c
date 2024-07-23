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
    FONT_TYPES,
};

enum FontName {
    FONT_RUBIK_MEDIUM_CELL_SIZED,
    FONT_RUBIK_MEDIUM_PRIMARY,
    FONT_RUBIK_MEDIUM_SECONDARY,
    FONT_NAMES,
};

void init_fonts(int window_height);
Font get_font(FontName name);
void free_fonts();
