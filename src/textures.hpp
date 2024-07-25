#pragma once

#include <SDL.h>

#include "fonts.hpp"

struct Texture {
    SDL_Surface *surface = nullptr;
    SDL_Texture *texture = nullptr;
    SDL_Rect area{};
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
};

static constexpr int TEXTURE_CELL_TYPES = TEXTURE_CELL_TRC + 1;

enum TextureCellSubtype {
    TEXTURE_CELL_COVERED,
    TEXTURE_CELL_COVERED_MINE,
    TEXTURE_CELL_FLAGGED_MINE,
    TEXTURE_CELL_TRIGGERED_MINE,
    TEXTURE_CELL_FLAG,
};

static constexpr int TEXTURE_CELL_SUBTYPES = TEXTURE_CELL_FLAG + 1;

enum TextureName {
    TEXTURE_GRID,
    TEXTURE_GAME_TIME_TEXT,
    TEXTURE_REMAINING_MINES_TEXT,
    TEXTURE_REMAINING_MINES_ICON,
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
};

static constexpr int COLORS_AMOUNT = COLOR_WHITE + 1;

struct Color {
    SDL_Color rgb;
    Uint32 value;
};

void init_textures(SDL_Renderer *renderer);
Texture get_cell_texture(TextureCellSubtype subtype, TextureCellType type);
Texture get_cell_number_texture(int number);
Texture &get_texture(TextureName name);
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
