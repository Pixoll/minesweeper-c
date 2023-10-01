#ifndef __GLOBAL_DEFINED

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "util.h"

#define __GLOBAL_DEFINED

// #define GAME_FRAME_DURATION (1 / 120)

extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern const int defaultWindowWidth;
extern const int defaultWindowHeight;
extern int windowWidth;
extern int windowHeight;

enum COLORS {
    COLOR_BG,
    COLOR_ACCENT,
    COLOR_WHITE,
    COLOR_GREY,
    COLOR_GRID_1,
    COLOR_GRID_2,
    COLOR_GRID_3,
    COLOR_GRID_4,
    COLOR_GRID_5,
    COLOR_GRID_6,
    COLOR_GRID_7,
    COLOR_GRID_8,
    COLORS_AMOUNT,
};
typedef enum COLORS COLORS;

extern Color colors[COLORS_AMOUNT];

void getWindowSize();
void initColors();

#endif
