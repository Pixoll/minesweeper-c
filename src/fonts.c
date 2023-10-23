#include "fonts.h"

#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>

#include "global.h"
#include "grid.h"
#include "util.h"

const char *fontPaths[FONT_TYPES] = {
    "assets/fonts/Rubik-Light.ttf",
    "assets/fonts/Rubik-Medium.ttf",
    "assets/fonts/Rubik-Regular.ttf",
    "assets/fonts/Rubik-Bold.ttf",
};

Font fontRubikMediumCellSized;
Font fontRubikMedium2;

bool fontsReady = false;

Font loadFont(FONT_TYPE type, int size) {
    TTF_Font *font = TTF_OpenFont(fontPaths[type], size);
    if (font == NULL) {
        printf("Error while loading font ID %d\n", type);
        throwSDLError("TTF_OpenFont");
    }
    return (Font){font, size};
}

void initFonts() {
    if (fontsReady) return;

    fontRubikMediumCellSized = loadFont(FONT_RUBIK_MEDIUM, game.measurements.cellSize * 0.5);
    fontRubikMedium2 = loadFont(FONT_RUBIK_MEDIUM, windowHeight * 0.02);

    fontsReady = true;
}

void freeFonts() {
    TTF_CloseFont(fontRubikMediumCellSized.font);
    TTF_CloseFont(fontRubikMedium2.font);
}
