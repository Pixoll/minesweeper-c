#ifndef __FONTS_DEFINED

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define __FONTS_DEFINED

enum FONT_TYPE {
    FONT_RUBIK_LIGHT,
    FONT_RUBIK_MEDIUM,
    FONT_RUBIK_REGULAR,
    FONT_RUBIK_BOLD,
    FONT_TYPES,
};
typedef enum FONT_TYPE FONT_TYPE;

extern TTF_Font *fontRubikMediumCellSized;

void initFonts();
void freeFonts();

#endif
