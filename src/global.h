#pragma once

#include <SDL.h>}

// #define GAME_FRAME_DURATION (1 / 120)

extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern const int defaultWindowWidth;
extern const int defaultWindowHeight;
extern int windowWidth;
extern int windowHeight;

enum COLOR {
    COLOR_BACKGROUND,
    COLOR_THEME,

    COLOR_GRID_1,
    COLOR_GRID_2,
    COLOR_GRID_3,
    COLOR_GRID_4,
    COLOR_GRID_5,
    COLOR_GRID_6,
    COLOR_GRID_7,
    COLOR_GRID_8,

    COLOR_FLAGGED_CELL,
    COLOR_FLAGGED_CELL_BG,
    COLOR_TRIGGERED_MINE,
    COLOR_TRIGGERED_MINE_BG,

    COLOR_BLACK,
    COLOR_DARK_GREY,
    COLOR_GREY,
    COLOR_LIGHT_GREY,
    COLOR_LIGHTER_GREY,
    COLOR_WHITE,

    COLORS_AMOUNT,
};
typedef enum COLOR COLOR;

typedef struct Color {
    SDL_Color rgb;
    Uint32 value;
} Color;

extern Color colors[COLORS_AMOUNT];

void getWindowSize();
void initColors();}
