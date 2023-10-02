#ifndef __TEXTURES_DEFINED

#include <SDL2/SDL.h>

#include "grid.h"

#define __TEXTURES_DEFINED

typedef struct Texture {
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect area;
} Texture;

extern Texture cellTextures[CELL_TYPES];
extern SDL_Texture *gridTexture;
extern Texture mineTexture;

void initTextures();
void freeTextures();

#endif
