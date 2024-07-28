#pragma once

#include <SDL.h>

#include "fonts.hpp"
#include "../core/game.hpp"

struct Texture {
    SDL_Surface *surface = nullptr;
    SDL_Texture *texture = nullptr;
    SDL_Rect area{0, 0, 0, 0};

    Texture() = default;

    Texture(SDL_Surface *_surface, SDL_Texture *_texture, const SDL_Rect _area) :
        surface(_surface),
        texture(_texture),
        area(_area) {}

    Texture(SDL_Texture *_texture, const SDL_Rect _area) :
        texture(_texture),
        area(_area) {}

    Texture(SDL_Renderer *renderer, const char *image_path);

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
        GRID,
        GAME_TIME_TEXT,
        REMAINING_MINES_TEXT,
        REMAINING_MINES_ICON,
    };
};

struct Color {
    SDL_Color rgb{0, 0, 0, 0};
    Uint32 value = 0;
    Color() = default;
    Color(const SDL_Surface *surface, const char *hex_color);

    enum Name {
        BACKGROUND,
        THEME,

        GRID_1,
        GRID_2,
        GRID_3,
        GRID_4,
        GRID_5,
        GRID_6,
        GRID_7,
        GRID_8,

        FLAGGED_CELL,
        FLAGGED_CELL_BG,
        TRIGGERED_MINE,
        TRIGGERED_MINE_BG,

        BLACK,
        DARK_GREY,
        GREY,
        LIGHT_GREY,
        LIGHTER_GREY,
        WHITE,
    };

    static constexpr int NAMES_AMOUNT = WHITE + 1;
};

void init_textures(SDL_Renderer *renderer, const Game::Measurements &measurements);
Texture get_cell_texture(Texture::CellSubtype subtype, Texture::CellType type);
Texture get_cell_number_texture(int number);
Texture &get_texture(Texture::Name name);
void free_textures();
void update_text_texture(
    SDL_Renderer *renderer,
    Texture *texture,
    Font::Name font_name,
    Color::Name color,
    const char *text
);

void init_colors(SDL_Window *window);
Color get_color(Color::Name name);
