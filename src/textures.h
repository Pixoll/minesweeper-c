#ifndef __TEXTURES_DEFINED

#include <SDL2/SDL.h>

#define __TEXTURES_DEFINED

typedef struct Texture {
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect area;
} Texture;

extern SDL_Texture *gridTexture;

extern Texture gridFillerHorizontalTexture;
extern Texture gridFillerVerticalTexture;
extern Texture gridFillerCornerTexture;

extern Texture gridFlaggedFillerHorizontalTexture;
extern Texture gridFlaggedFillerVerticalTexture;
extern Texture gridFlaggedFillerCornerTexture;

extern Texture cellNumbersTextures[8];
extern Texture cellCoveredTexture;

extern Texture cellCoveredMineTexture;
extern Texture cellFlaggedMineTexture;
extern Texture cellTriggeredMineTexture;
extern Texture cellFlagTexture;

void initTextures();
void freeTextures();

#endif
