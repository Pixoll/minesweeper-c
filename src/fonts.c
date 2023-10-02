#include "fonts.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>

#include "grid.h"
#include "util.h"

const char *fontPaths[FONT_TYPES] = {
    "assets/fonts/Rubik-Light.ttf",
    "assets/fonts/Rubik-Medium.ttf",
    "assets/fonts/Rubik-Regular.ttf",
    "assets/fonts/Rubik-Bold.ttf",
};

TTF_Font *fontRubikMediumCellSized;

bool fontsReady = false;

TTF_Font *getFont(FONT_TYPE type, int size) {
    TTF_Font *font = TTF_OpenFont(fontPaths[type], size);
    if (font == NULL) {
        printf("Error while loading font ID %d\n", type);
        throwSDLError("TTF_OpenFont");
    }
    return font;
}

void initFonts() {
    if (fontsReady) return;

    fontRubikMediumCellSized = getFont(FONT_RUBIK_MEDIUM, gridMeasurements.cellSize);

    fontsReady = true;
}

void freeFonts() {
    TTF_CloseFont(fontRubikMediumCellSized);
}
