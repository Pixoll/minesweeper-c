#include "util.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "global.h"

int randomBetween(const int min, const int max) {
    return (rand() % (max - min + 1)) + min;
}

Color mapColor(const SDL_Surface *surface, const char *hexColor) {
    if (hexColor[0] == '#') hexColor++; // shift left once
    const int rgb = strtol(hexColor, NULL, 16);
    const int r = (rgb >> 16) & 0xff;
    const int g = (rgb >> 8) & 0xff;
    const int b = rgb & 0xff;

    Color color;
    color.rgb = (SDL_Color){r, g, b, 255};
    color.value = SDL_MapRGB(surface->format, r, g, b);
    return color;
}

SDL_Surface *createSurface(const int width, const int height) {
    SDL_PixelFormat *pixelFormat = SDL_GetWindowSurface(window)->format;

    SDL_Surface *surface = SDL_CreateRGBSurface(0, width, height, 32, pixelFormat->Rmask,
                                                pixelFormat->Gmask, pixelFormat->Bmask, pixelFormat->Amask);
    return surface;
}

SDL_Surface *createColoredSurface(const int width, const int height, const COLOR color) {
    SDL_Surface *surface = createSurface(width, height);
    SDL_Rect area = rectangle(0, 0, width, height);
    const Uint32 surfaceColor = colors[color].value;
    SDL_FillRect(surface, &area, surfaceColor);
    return surface;
}

void throwSDLError(const char *functionName) {
    printf("%s() failed: %s\n", functionName, SDL_GetError());
    exit(1);
}
