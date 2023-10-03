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

char *substr(const char *buffer, const unsigned int from, const unsigned int to) {
    const int ogSize = strlen(buffer);
    const int start = min(from, to);
    const int end = min(max(to, from), ogSize);
    if (start >= ogSize || start == end)
        return "";

    const int size = end - start + 1;
    char *result = malloc(size);
    for (int i = 0; i < size - 1; i++)
        result[i] = buffer[start + i];
    result[size - 1] = '\0';
    return result;
}

Color mapColor(const SDL_Surface *surface, const char *hexColor) {
    Color color;
    const int start = hexColor[0] == '#' ? 1 : 0;
    const int r = strtol(substr(hexColor, start, start + 2), NULL, 16),
              g = strtol(substr(hexColor, start + 2, start + 4), NULL, 16),
              b = strtol(substr(hexColor, start + 4, start + 6), NULL, 16);

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

void throwSDLError(const char *functionName) {
    printf("%s() failed: %s\n", functionName, SDL_GetError());
    exit(1);
}
