#ifndef __TEXTURES_DEFINED

#include <SDL.h>

#include "fonts.h"
#include "global.h"

#define __TEXTURES_DEFINED

typedef struct Texture {
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect area;
} Texture;

enum TEXTURE_CELL_TYPE {
    TEXTURE_CELL_NO_SIDES,
    TEXTURE_CELL_B,
    TEXTURE_CELL_BL,
    TEXTURE_CELL_BLC,
    TEXTURE_CELL_BLR,
    TEXTURE_CELL_BLR_BLC,
    TEXTURE_CELL_BLR_BLCRC,
    TEXTURE_CELL_BLR_BRC,
    TEXTURE_CELL_BR,
    TEXTURE_CELL_BRC,
    TEXTURE_CELL_BT,
    TEXTURE_CELL_L,
    TEXTURE_CELL_LR,
    TEXTURE_CELL_R,
    TEXTURE_CELL_T,
    TEXTURE_CELL_TBL,
    TEXTURE_CELL_TBL_BLC,
    TEXTURE_CELL_TBL_TLC,
    TEXTURE_CELL_TBL_TLC_BLC,
    TEXTURE_CELL_TBLR,
    TEXTURE_CELL_TBLR_BLC,
    TEXTURE_CELL_TBLR_BLCRC,
    TEXTURE_CELL_TBLR_BRC,
    TEXTURE_CELL_TBLR_TLC,
    TEXTURE_CELL_TBLR_TLC_BLC,
    TEXTURE_CELL_TBLR_TLC_BLCRC,
    TEXTURE_CELL_TBLR_TLC_BRC,
    TEXTURE_CELL_TBLR_TLCRC,
    TEXTURE_CELL_TBLR_TLCRC_BLC,
    TEXTURE_CELL_TBLR_TLCRC_BLCRC,
    TEXTURE_CELL_TBLR_TLCRC_BRC,
    TEXTURE_CELL_TBLR_TRC,
    TEXTURE_CELL_TBLR_TRC_BLC,
    TEXTURE_CELL_TBLR_TRC_BLCRC,
    TEXTURE_CELL_TBLR_TRC_BRC,
    TEXTURE_CELL_TBR,
    TEXTURE_CELL_TBR_BRC,
    TEXTURE_CELL_TBR_TRC,
    TEXTURE_CELL_TBR_TRC_BRC,
    TEXTURE_CELL_TL,
    TEXTURE_CELL_TLC,
    TEXTURE_CELL_TLR,
    TEXTURE_CELL_TLR_TLC,
    TEXTURE_CELL_TLR_TLCRC,
    TEXTURE_CELL_TLR_TRC,
    TEXTURE_CELL_TR,
    TEXTURE_CELL_TRC,
    TEXTURE_CELL_TYPES,
};

/**
 * T - Top side
 * B - Bottom side
 * L - Left side
 * R - Right side
 * C - Corner
 *
 * E.g. *_TBLR_TLCRC_BLC:
 * - Top Bottom Left Right sides
 * - Top Left Corner, Top Right Corner
 * - Bottom Left Corner
 */
typedef enum TEXTURE_CELL_TYPE TEXTURE_CELL_TYPE;

extern const TEXTURE_CELL_TYPE textureCellSideTypeOrder[16];
extern const TEXTURE_CELL_TYPE textureCellCornerTypeOrder[33];

extern Texture gridTexture;

extern Texture cellCoveredTextures[TEXTURE_CELL_TYPES];
extern Texture cellCoveredMineTextures[TEXTURE_CELL_TYPES];
extern Texture cellFlaggedMineTextures[TEXTURE_CELL_TYPES];
extern Texture cellTriggeredMineTextures[TEXTURE_CELL_TYPES];
extern Texture cellFlagTextures[TEXTURE_CELL_TYPES];

extern Texture cellNumbersTextures[8];

extern Texture gameTimeTextTexture;
extern Texture remainingMinesTextTexture;
extern Texture remainingMinesIconTexture;

void initTextures();
void freeTextures();
void updateTextTexture(Texture *texture, Font font, COLOR color, const char *text);

#endif
