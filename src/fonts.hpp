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

extern Font fontRubikMediumCellSized;
extern Font fontRubikMediumPrimary;
extern Font fontRubikMediumSecondary;

void initFonts();
void freeFonts();
