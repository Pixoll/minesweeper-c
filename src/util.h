#ifndef __UTIL_DEFINED

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "global.h"

#define __UTIL_DEFINED

// General purpose

#define isPow2(a) ((a) > 0 && !((a) & ((a) - 1)))
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
// #define pxToPt(px) ((px)*0.75)

int randomBetween(int min, int max);
int intLog2(int x);

// SDL related

#define rectangle(x, y, width, height) ((SDL_Rect){x, y, width, height})

Color mapColor(const SDL_Surface *surface, const char *hexColor);
SDL_Surface *createSurface(int width, int height);
SDL_Texture *createTexture(int width, int height, int access);
SDL_Surface *createColoredSurface(int width, int height, COLOR color);
void throwSDLError(const char *functionName);

#endif
