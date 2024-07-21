#include "util.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "global.h"

int randomBetween(const int min, const int max) {
    return rand() % (max - min + 1) + min;
}

int intLog2(int x) {
    int log2 = 0;
    while (x >>= 1) log2++;
    return log2;
}

int intLength(int value) {
    int length = 1;
    while (value > 9) {
        length++;
        value /= 10;
    }
    return length;
}

char *getTimeString(const int seconds) {
    const int sec = seconds % 60;
    const int min = seconds / 60;

    const int timeLength = intLength(sec) + intLength(min) + 4;
    char *timeString = calloc(timeLength, sizeof(char));

    if (min == 0)
        snprintf(timeString, timeLength, "%dS", sec);
    else
        snprintf(timeString, timeLength, "%dM %dS", min, sec);

    return timeString;
}

Color mapColor(const SDL_Surface *surface, const char *hexColor) {
    if (hexColor[0] == '#') hexColor++;  // shift left once
    const int rgb = strtol(hexColor, NULL, 16);
    const int r = rgb >> 16 & 0xff;
    const int g = rgb >> 8 & 0xff;
    const int b = rgb & 0xff;

    Color color;
    color.rgb = (SDL_Color){r, g, b, 255};
    color.value = SDL_MapRGB(surface->format, r, g, b);
    return color;
}

SDL_Surface *createSurface(const int width, const int height) {
    const Uint32 pixelFormat = SDL_GetWindowSurface(window)->format->format;
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, pixelFormat);
    return surface;
}

SDL_Texture *createTexture(const int width, const int height, const int access) {
    const Uint32 pixelFormat = SDL_GetWindowSurface(window)->format->format;
    SDL_Texture *texture = SDL_CreateTexture(renderer, pixelFormat, access, width, height);
    return texture;
}

SDL_Surface *createColoredSurface(const int width, const int height, const COLOR color) {
    SDL_Surface *surface = createSurface(width, height);
    const SDL_Rect area = rectangle(0, 0, width, height);
    const Uint32 surfaceColor = colors[color].value;
    SDL_FillRect(surface, &area, surfaceColor);
    return surface;
}

void throwSDLError(const char *functionName) {
    printf("%s() failed: %s\n", functionName, SDL_GetError());
    exit(1);
}
