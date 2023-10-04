#ifndef __TEXTURES_DEFINED

#include <SDL2/SDL.h>

#define __TEXTURES_DEFINED

typedef struct Texture {
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect area;
} Texture;

extern Texture cellNumbersTextures[8];
extern SDL_Texture *gridTexture;
extern Texture coveredCellTexture;
extern Texture cellCoveredMineTexture;
extern Texture cellFlaggedMineTexture;
extern Texture cellFlagTexture;

void initTextures();
void freeTextures();

#endif
