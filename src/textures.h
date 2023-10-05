#ifndef __TEXTURES_DEFINED

#include <SDL2/SDL.h>

#define __TEXTURES_DEFINED

typedef struct Texture {
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect area;
} Texture;

extern SDL_Texture *gridTexture;

extern Texture cellNumbersTextures[8];
extern Texture cellCoveredTexture;

extern Texture cellInterFillingHorizontalTexture;
extern Texture cellInterFillingVerticalTexture;
extern Texture cellInterFillingCornerTexture;

extern Texture cellFlaggedInterFillingHorizontalTexture;
extern Texture cellFlaggedInterFillingVerticalTexture;
extern Texture cellFlaggedInterFillingCornerTexture;

extern Texture cellCoveredMineTexture;
extern Texture cellFlaggedMineTexture;
extern Texture cellTriggeredMineTexture;
extern Texture cellFlagTexture;

void initTextures();
void freeTextures();

#endif
