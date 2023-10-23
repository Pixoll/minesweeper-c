#ifndef __FONTS_DEFINED

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

typedef struct Font {
    TTF_Font *font;
    int size;
} Font;

extern Font fontRubikMediumCellSized;
// 2% of window height
extern Font fontRubikMedium2;

void initFonts();
void freeFonts();

#endif
