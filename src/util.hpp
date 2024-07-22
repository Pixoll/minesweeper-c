#pragma once

#include <SDL.h>

#include "global.hpp"

// General purpose

// #define pxToPt(px) ((px)*0.75)

int randomBetween(int min, int max);
int isPow2(int x);
int intLog2(int x);
int intLength(int value);
char *getTimeString(int seconds);

// SDL related

Color mapColor(const SDL_Surface *surface, const char *hexColor);
SDL_Surface *createSurface(int width, int height);
SDL_Texture *createTexture(int width, int height, int access);
SDL_Surface *createColoredSurface(int width, int height, COLOR color);
void throwSDLError(const char *functionName);
