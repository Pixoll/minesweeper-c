#pragma once

#include <SDL.h>

#include "texture.hpp"
#include "../core/game.hpp"

class GameTexture {
public:
    /**
     * T - Top side
     * B - Bottom side
     * L - Left side
     * R - Right side
     * C - Corner
     *
     * E.g. TBLR_TLCRC_BLC:
     * - Top Bottom Left Right sides
     * - Top Left Corner, Top Right Corner
     * - Bottom Left Corner
     */
    enum CellType {
        CELL_NO_SIDES,
        CELL_B,
        CELL_BL,
        CELL_BLC,
        CELL_BLR,
        CELL_BLR_BLC,
        CELL_BLR_BLCRC,
        CELL_BLR_BRC,
        CELL_BR,
        CELL_BRC,
        CELL_BT,
        CELL_L,
        CELL_LR,
        CELL_R,
        CELL_T,
        CELL_TBL,
        CELL_TBL_BLC,
        CELL_TBL_TLC,
        CELL_TBL_TLC_BLC,
        CELL_TBLR,
        CELL_TBLR_BLC,
        CELL_TBLR_BLCRC,
        CELL_TBLR_BRC,
        CELL_TBLR_TLC,
        CELL_TBLR_TLC_BLC,
        CELL_TBLR_TLC_BLCRC,
        CELL_TBLR_TLC_BRC,
        CELL_TBLR_TLCRC,
        CELL_TBLR_TLCRC_BLC,
        CELL_TBLR_TLCRC_BLCRC,
        CELL_TBLR_TLCRC_BRC,
        CELL_TBLR_TRC,
        CELL_TBLR_TRC_BLC,
        CELL_TBLR_TRC_BLCRC,
        CELL_TBLR_TRC_BRC,
        CELL_TBR,
        CELL_TBR_BRC,
        CELL_TBR_TRC,
        CELL_TBR_TRC_BRC,
        CELL_TL,
        CELL_TLC,
        CELL_TLR,
        CELL_TLR_TLC,
        CELL_TLR_TLCRC,
        CELL_TLR_TRC,
        CELL_TR,
        CELL_TRC,
    };

    static constexpr int CELL_TYPES = CELL_TRC + 1;

    enum CellSubtype {
        CELL_COVERED,
        CELL_COVERED_MINE,
        CELL_FLAGGED_MINE,
        CELL_TRIGGERED_MINE,
        CELL_FLAG,
    };

    static constexpr int CELL_SUBTYPES = CELL_FLAG + 1;

    enum Name {
        H_GRID_LINE,
        V_GRID_LINE,
        GAME_TIME_TEXT,
        REMAINING_MINES_TEXT,
        REMAINING_MINES_ICON,
        MOUSE_LEFT_ICON,
        MOUSE_LEFT_TEXT,
        MOUSE_RIGHT_ICON,
        MOUSE_RIGHT_TEXT,
        BACK_BUTTON,
    };
};

void init_game_textures(SDL_Renderer *renderer, const Game::Measurements &measurements, int window_height);

Texture get_cell_texture(GameTexture::CellSubtype subtype, GameTexture::CellType type);
Texture get_cell_number_texture(int number);
Texture &get_game_texture(GameTexture::Name name);

void free_game_textures();
