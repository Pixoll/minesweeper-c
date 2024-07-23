#pragma once

#include <SDL.h>

#include "fonts.hpp"

struct Texture {
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect area;
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
enum TextureCellType {
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

enum ColorName {
    COLOR_BACKGROUND,
    COLOR_THEME,

    COLOR_GRID_1,
    COLOR_GRID_2,
    COLOR_GRID_3,
    COLOR_GRID_4,
    COLOR_GRID_5,
    COLOR_GRID_6,
    COLOR_GRID_7,
    COLOR_GRID_8,

    COLOR_FLAGGED_CELL,
    COLOR_FLAGGED_CELL_BG,
    COLOR_TRIGGERED_MINE,
    COLOR_TRIGGERED_MINE_BG,

    COLOR_BLACK,
    COLOR_DARK_GREY,
    COLOR_GREY,
    COLOR_LIGHT_GREY,
    COLOR_LIGHTER_GREY,
    COLOR_WHITE,

    COLORS_AMOUNT,
};

struct Color {
    SDL_Color rgb;
    Uint32 value;
};

extern const TextureCellType texture_cell_side_type_order[16];
extern const TextureCellType texture_cell_corner_type_order[33];

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

void init_textures(SDL_Renderer *renderer);
void free_textures();
void update_text_texture(
    SDL_Renderer *renderer,
    Texture *texture,
    FontName font_name,
    ColorName color,
    const char *text
);

void init_colors(SDL_Window *window);
Color get_color(ColorName name);
