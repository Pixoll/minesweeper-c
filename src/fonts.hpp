#pragma once

#include <SDL_ttf.h>

typedef struct Font {
    TTF_Font *font;
    int size;
} Font;

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
};

void init_fonts();
Font get_font(FontName name);
void free_fonts();
