#ifndef __TEXTURES_DEFINED

#include <SDL2/SDL.h>

#include "grid.h"

#define __TEXTURES_DEFINED

typedef struct Texture {
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect area;
} Texture;

extern Texture cellNumbersTextures[7];
extern SDL_Texture *gridTexture;
extern Texture cellMineTexture;

void initTextures();
void freeTextures();

#endif
