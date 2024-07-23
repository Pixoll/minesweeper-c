#pragma once

#include <SDL.h>

#include "fonts.hpp"
#include "global.hpp"

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

extern const TEXTURE_CELL_TYPE texture_cell_side_type_order[16];
extern const TEXTURE_CELL_TYPE texture_cell_corner_type_order[33];

extern Texture grid_texture;

extern Texture cell_covered_textures[TEXTURE_CELL_TYPES];
extern Texture cell_covered_mine_textures[TEXTURE_CELL_TYPES];
extern Texture cell_flagged_mine_textures[TEXTURE_CELL_TYPES];
extern Texture cell_triggered_mine_textures[TEXTURE_CELL_TYPES];
extern Texture cell_flag_textures[TEXTURE_CELL_TYPES];

extern Texture cell_numbers_textures[8];

extern Texture game_time_text_texture;
extern Texture remaining_mines_text_texture;
extern Texture remaining_mines_icon_texture;

void init_textures();
void free_textures();
void update_text_texture(Texture *texture, Font font, COLOR color, const char *text);
