#pragma once

#include <cstdlib>
#include <SDL.h>
#include <SDL_image.h>

#include "fonts.hpp"
#include "../core/game.hpp"

constexpr SDL_Rect NULL_RECT = {0, 0, 0, 0};

class Texture {
    SDL_Surface *m_surface = nullptr;
    SDL_Texture *m_texture = nullptr;
    SDL_Rect m_area{0, 0, 0, 0};

public:
    Texture() = default;

    Texture(SDL_Surface *surface, SDL_Texture *texture, const SDL_Rect area) :
        m_surface(surface),
        m_texture(texture),
        m_area(area) {}

    Texture(SDL_Texture *texture, const SDL_Rect area) : Texture(nullptr, texture, area) {}

    Texture(SDL_Renderer *renderer, const char *image_path) {
        m_surface = IMG_Load(image_path);
        m_texture = SDL_CreateTextureFromSurface(renderer, m_surface);
        m_area = {0, 0, m_surface->w, m_surface->h};
    }

    Texture(SDL_Renderer *renderer, const char *image_path, const SDL_Rect area) : m_area(area) {
        m_surface = IMG_Load(image_path);
        m_texture = SDL_CreateTextureFromSurface(renderer, m_surface);
    }

    ~Texture() = default;

    [[nodiscard]] int get_h() const {
        return m_area.h;
    }

    [[nodiscard]] int get_w() const {
        return m_area.w;
    }

    [[nodiscard]] const SDL_Rect &get_area() const {
        return m_area;
    }

    void set_size(const int size) {
        m_area.h = size;
        m_area.w = size;
    }

    void set_position(const int x, const int y) {
        m_area.x = x;
        m_area.y = y;
    }

    void move(const int x_pixels, const int y_pixels) {
        m_area.x += x_pixels;
        m_area.y += y_pixels;
    }

    void set_color_mod(const SDL_Color color) const {
        SDL_SetTextureColorMod(m_texture, color.r, color.g, color.b);
    }

    void set_as_render_target(SDL_Renderer *renderer, const SDL_BlendMode blend_mode = SDL_BLENDMODE_BLEND) const {
        SDL_SetRenderTarget(renderer, m_texture);
        SDL_SetTextureBlendMode(m_texture, blend_mode);
    }

    void render(SDL_Renderer *renderer, const SDL_Rect source, const SDL_Rect destination = NULL_RECT) const {
        SDL_RenderCopy(
            renderer,
            m_texture,
            is_null_rect(source) ? nullptr : &source,
            is_null_rect(destination) ? nullptr : &destination
        );
    }

    void destroy() {
        if (m_texture == nullptr)
            return;

        SDL_FreeSurface(m_surface);
        SDL_DestroyTexture(m_texture);
        m_surface = nullptr;
        m_texture = nullptr;
    }

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

private:
    static bool is_null_rect(const SDL_Rect &rect) {
        return rect.x == 0 && rect.y == 0 && rect.h == 0 && rect.w == 0;
    }
};

struct Color {
    SDL_Color rgb{0, 0, 0, 0};
    Uint32 value = 0;

    Color() = default;

    Color(const SDL_Surface *surface, const char *hex_color) {
        if (hex_color[0] == '#')
            hex_color++;  // shift left once

        const int raw_rgb = strtol(hex_color, nullptr, 16);
        const Uint8 r = raw_rgb >> 16 & 0xff;
        const Uint8 g = raw_rgb >> 8 & 0xff;
        const Uint8 b = raw_rgb & 0xff;

        rgb = {r, g, b, 255};
        value = SDL_MapRGB(surface->format, r, g, b);
    }

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
    Texture &texture,
    Font::Name font_name,
    Color::Name color,
    const char *text
);

void init_colors(SDL_Window *window);
Color get_color(Color::Name name);
