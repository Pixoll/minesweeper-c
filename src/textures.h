#ifndef __TEXTURES_DEFINED

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "grid.h"

#define __TEXTURES_DEFINED

typedef struct Texture {
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect area;
} Texture;

extern Texture cellTextures[CELL_TYPES];
extern SDL_Texture *gridTexture;

void initTextures(TTF_Font *RubikMedium, int cellSize, int gridLineWidth, int gridXOffset, int gridYOffset, int gridWidth, int gridHeight);
void freeTextures();

#endif
