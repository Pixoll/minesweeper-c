#pragma once

#include <SDL_ttf.h>

enum FONT_TYPE {
    FONT_RUBIK_LIGHT,
    FONT_RUBIK_MEDIUM,
    FONT_RUBIK_REGULAR,
    FONT_RUBIK_BOLD,
    FONT_TYPES,
};

typedef FONT_TYPE FONT_TYPE;

typedef struct Font {
    TTF_Font *font;
    int size;
} Font;

extern Font font_rubik_medium_cell_sized;
extern Font font_rubik_medium_primary;
extern Font font_rubik_medium_secondary;

void init_fonts();
void free_fonts();
