#ifndef __UTIL_DEFINED

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define __UTIL_DEFINED

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
// #define pxToPt(px) ((px)*0.75)

// General purpose

int randomBetween(int min, int max);

// String related

char *substr(const char *buffer, unsigned int start, unsigned int end);

// SDL related

#define rectangle(x, y, width, height) ((SDL_Rect){x, y, width, height})

typedef struct Color {
    SDL_Color rgb;
    Uint32 value;
} Color;

Color mapColor(const SDL_Surface *surface, const char *hexColor);
void throwSDLError(const char *functionName);

#endif
