#ifndef __FONTS_DEFINED

#include <SDL_ttf.h>

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
extern Font fontRubikMediumPrimary;
extern Font fontRubikMediumSecondary;

void initFonts();
void freeFonts();

#endif
