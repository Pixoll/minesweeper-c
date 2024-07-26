#pragma once

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

#include "screen.hpp"
#include "../core/game.hpp"
#include "../graphics/fonts.hpp"
#include "../graphics/textures.hpp"

class Engine;

class GameScreen final : virtual public Screen {
    Engine *m_engine;
    SDL_Window *m_window;
    SDL_Renderer *m_renderer;
    int m_window_width;
    int m_window_height;
    Game m_game;
    bool m_placed_mines = false;
    time_t m_last_game_time_drawn = 0;
    int m_remaining_mines = 0;

    static constexpr TextureCellType m_texture_cell_side_type_order[16] = {
        TEXTURE_CELL_NO_SIDES,
        TEXTURE_CELL_R,
        TEXTURE_CELL_L,
        TEXTURE_CELL_LR,
        TEXTURE_CELL_B,
        TEXTURE_CELL_BR,
        TEXTURE_CELL_BL,
        TEXTURE_CELL_BLR,
        TEXTURE_CELL_T,
        TEXTURE_CELL_TR,
        TEXTURE_CELL_TL,
        TEXTURE_CELL_TLR,
        TEXTURE_CELL_BT,
        TEXTURE_CELL_TBR,
        TEXTURE_CELL_TBL,
        TEXTURE_CELL_TBLR,
    };

    static constexpr TextureCellType m_texture_cell_corner_type_order[33] = {
        TEXTURE_CELL_TBLR_BRC,
        TEXTURE_CELL_TBLR_BLC,
        TEXTURE_CELL_TBLR_BLCRC,
        TEXTURE_CELL_TBLR_TRC,
        TEXTURE_CELL_TBLR_TRC_BRC,
        TEXTURE_CELL_TBLR_TRC_BLC,
        TEXTURE_CELL_TBLR_TRC_BLCRC,
        TEXTURE_CELL_TBLR_TLC,
        TEXTURE_CELL_TBLR_TLC_BRC,
        TEXTURE_CELL_TBLR_TLC_BLC,
        TEXTURE_CELL_TBLR_TLC_BLCRC,
        TEXTURE_CELL_TBLR_TLCRC,
        TEXTURE_CELL_TBLR_TLCRC_BRC,
        TEXTURE_CELL_TBLR_TLCRC_BLC,
        TEXTURE_CELL_TBLR_TLCRC_BLCRC,

        TEXTURE_CELL_BLR_BRC,
        TEXTURE_CELL_BLR_BLC,
        TEXTURE_CELL_BLR_BLCRC,
        TEXTURE_CELL_TBR_BRC,
        TEXTURE_CELL_TLR_TRC,
        TEXTURE_CELL_TBL_BLC,
        TEXTURE_CELL_TBR_TRC,
        TEXTURE_CELL_TBR_TRC_BRC,
        TEXTURE_CELL_TLR_TLC,
        TEXTURE_CELL_NO_SIDES,
        TEXTURE_CELL_NO_SIDES,
        TEXTURE_CELL_TBL_TLC,
        TEXTURE_CELL_TLR_TLCRC,
        TEXTURE_CELL_TBL_TLC_BLC,

        TEXTURE_CELL_BRC,
        TEXTURE_CELL_BLC,
        TEXTURE_CELL_TRC,
        TEXTURE_CELL_TLC,
    };

public:
    explicit GameScreen(Engine *engine) :
        m_engine(engine),
        m_window(engine->get_window()),
        m_renderer(engine->get_renderer()),
        m_window_width(engine->get_window_width()),
        m_window_height(engine->get_window_height()),
        m_game(15, 20, 50, engine->get_window_width(), engine->get_window_height()) {
        init_colors(m_window);

        const auto [r, g, b, a] = get_color(COLOR_BACKGROUND).rgb;
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);

        const Game::Measurements &measurements = m_game.get_measurements();

        init_fonts(m_window_height, measurements.cell_size);
        init_textures(m_renderer, measurements);
    }

    ~GameScreen() override = default;

    void run_logic(const SDL_Event &event) override {
        if (event.type != SDL_MOUSEBUTTONDOWN || m_game.is_over())
            return;

        int click_x, click_y;
        SDL_GetMouseState(&click_x, &click_y);
        const auto [x, y, inside] = m_game.calculate_grid_cell(click_x, click_y);

        if (!inside)
            return;

        switch (event.button.button) {
            case SDL_BUTTON_LEFT: {
                if (!m_placed_mines) {
                    m_game.place_grid_mines(x, y);
                    m_placed_mines = true;
                }

                m_game.reveal_cell(x, y);
                break;
            }

            case SDL_BUTTON_RIGHT: {
                if (!m_placed_mines)
                    break;

                m_game.toggle_cell_flag(x, y);
                break;
            }

            default:
                break;
        }
    }

    void render() override {
        SDL_RenderClear(m_renderer);

        draw_grid();
        draw_remaining_mines();
        draw_game_time();

        SDL_RenderPresent(m_renderer);
    }

private:
    void draw_grid() const {
        const Texture &grid_texture = get_texture(TEXTURE_GRID);

        const int rows = m_game.get_rows();
        const int columns = m_game.get_columns();

        const Game::Measurements &measurements = m_game.get_measurements();
        const int cell_size = measurements.cell_size;
        const int grid_x_offset = measurements.grid_x_offset;
        const int grid_y_offset = measurements.grid_y_offset;

        // Draw grid
        SDL_RenderCopy(m_renderer, grid_texture.texture, nullptr, &grid_texture.area);

        // Draw cells
        for (int i = 0; i < columns; i++) {
            const int x = grid_x_offset + cell_size * i;

            for (int j = 0; j < rows; j++) {
                const int y = grid_y_offset + cell_size * j;
                const Game::GridCell cell = m_game.get_grid_cell(i, j);

                if (cell.type == Game::CELL_0 && cell.revealed)
                    continue;

                const TextureCellType cell_type = get_cell_type(i, j, cell.flagged, cell.revealed);
                Texture cell_texture = get_grid_cell_texture(cell, cell_type);
                cell_texture.area.x += x;
                cell_texture.area.y += y;

                SDL_RenderCopy(m_renderer, cell_texture.texture, nullptr, &cell_texture.area);
            }
        }
    }

    static int int_length(int value) {
        int length = 1;
        while (value > 9) {
            length++;
            value /= 10;
        }
        return length;
    }

    void draw_remaining_mines() {
        Texture &remaining_mines_text_texture = get_texture(TEXTURE_REMAINING_MINES_TEXT);
        Texture &remaining_mines_icon_texture = get_texture(TEXTURE_REMAINING_MINES_ICON);

        const int current_remaining = m_game.get_total_mines() - m_game.get_flagged_mines();

        if (m_remaining_mines != current_remaining) {
            m_remaining_mines = current_remaining;
            char remaining_string[int_length(current_remaining) + 1];
            itoa(current_remaining, remaining_string, 10);
            update_text_texture(
                m_renderer,
                &remaining_mines_text_texture,
                FONT_RUBIK_MEDIUM_PRIMARY,
                COLOR_WHITE,
                remaining_string
            );
        }

        remaining_mines_icon_texture.area.x = 10;
        remaining_mines_icon_texture.area.y = 10;
        SDL_RenderCopy(m_renderer, remaining_mines_icon_texture.texture, nullptr, &remaining_mines_icon_texture.area);

        remaining_mines_text_texture.area.x = remaining_mines_icon_texture.area.w + 20;
        remaining_mines_text_texture.area.y = 10
                + (remaining_mines_icon_texture.area.h - remaining_mines_text_texture.area.h) / 2;
        SDL_RenderCopy(m_renderer, remaining_mines_text_texture.texture, nullptr, &remaining_mines_text_texture.area);
    }

    static std::string get_time_string(const int seconds) {
        using std::string, std::to_string;

        const int sec = seconds % 60;
        const int min = seconds / 60;

        string time_string;

        if (min == 0)
            time_string += to_string(sec) + "S";
        else
            time_string += to_string(min) + "M " + to_string(sec) + "S";

        return time_string;
    }

    void draw_game_time() {
        using std::string;

        Texture &game_time_text_texture = get_texture(TEXTURE_GAME_TIME_TEXT);
        const time_t start_time = m_game.get_start_time();

        if (start_time == 0)
            return;

        const time_t now = time(nullptr);
        if (m_last_game_time_drawn == 0 || (!m_game.is_over() && m_last_game_time_drawn < now)) {
            m_last_game_time_drawn = now;
            const string time_string = get_time_string(now - start_time);
            update_text_texture(
                m_renderer,
                &game_time_text_texture,
                FONT_RUBIK_MEDIUM_SECONDARY,
                COLOR_LIGHTER_GREY,
                time_string.c_str()
            );
        }

        game_time_text_texture.area.x = 10;
        game_time_text_texture.area.y = game_time_text_texture.area.h + 20;
        SDL_RenderCopy(m_renderer, game_time_text_texture.texture, nullptr, &game_time_text_texture.area);
    }

    [[nodiscard]] Texture get_grid_cell_texture(const Game::GridCell cell, const TextureCellType type) const {
        if (m_game.is_over() && !m_game.has_won() && cell.type == Game::CELL_MINE) {
            if (cell.flagged)
                return get_cell_texture(TEXTURE_CELL_FLAGGED_MINE, type);

            if (cell.revealed)
                return get_cell_texture(TEXTURE_CELL_TRIGGERED_MINE, type);

            return get_cell_texture(TEXTURE_CELL_COVERED_MINE, type);
        }

        if (cell.flagged)
            return get_cell_texture(TEXTURE_CELL_FLAG, type);

        if (!cell.revealed)
            return get_cell_texture(TEXTURE_CELL_COVERED, type);

        return get_cell_number_texture(cell.type - Game::CELL_1);
    }

    [[nodiscard]] bool verify_cell(const int x, const int y, const bool flagged) const {
        const auto [type, cell_flagged, revealed] = m_game.get_grid_cell(x, y);
        return !revealed && flagged == cell_flagged;
    }

    static bool verify_corners_with_mask(const int corners, const int mask) {
        const bool bits_outside_of_mask = corners & ~mask;
        const bool bits_in_mask = corners & mask;
        return !bits_outside_of_mask && bits_in_mask;
    }

    static int is_pow2(const int x) {
        return x > 0 && !(x & x - 1);
    }

    static int int_log2(int x) {
        int log2 = 0;
        while (x >>= 1)
            log2++;

        return log2;
    }

    [[nodiscard]] TextureCellType get_cell_type(
        const int x,
        const int y,
        const bool flagged,
        const bool revealed
    ) const {
        if (revealed)
            return TEXTURE_CELL_NO_SIDES;

        const int rows = m_game.get_rows();
        const int columns = m_game.get_columns();

        const bool T = y - 1 >= 0 && verify_cell(x, y - 1, flagged);
        const bool B = y + 1 <= rows - 1 && verify_cell(x, y + 1, flagged);
        const bool L = x - 1 >= 0 && verify_cell(x - 1, y, flagged);
        const bool R = x + 1 <= columns - 1 && verify_cell(x + 1, y, flagged);
        const bool TLC = T && L && verify_cell(x - 1, y - 1, flagged);
        const bool TRC = T && R && verify_cell(x + 1, y - 1, flagged);
        const bool BLC = B && L && verify_cell(x - 1, y + 1, flagged);
        const bool BRC = B && R && verify_cell(x + 1, y + 1, flagged);

        const int TBLR = T << 3 | B << 2 | L << 1 | R;
        const int TLR_BLR_C = TLC << 3 | TRC << 2 | BLC << 1 | BRC;

        if (TLR_BLR_C == 0b0000)
            return m_texture_cell_side_type_order[TBLR];
        if (TBLR == 0b1111)
            return m_texture_cell_corner_type_order[TLR_BLR_C - 1];

        if (TBLR == 0b0111 && verify_corners_with_mask(TLR_BLR_C, 0b0011))
            return m_texture_cell_corner_type_order[TLR_BLR_C + 14];
        if (TBLR == 0b1011 && verify_corners_with_mask(TLR_BLR_C, 0b1100))
            return m_texture_cell_corner_type_order[TLR_BLR_C + 15];
        if (TBLR == 0b1101 && verify_corners_with_mask(TLR_BLR_C, 0b0101))
            return m_texture_cell_corner_type_order[TLR_BLR_C + 17];
        if (TBLR == 0b1110 && verify_corners_with_mask(TLR_BLR_C, 0b1010))
            return m_texture_cell_corner_type_order[TLR_BLR_C + 18];

        if (is_pow2(TLR_BLR_C))
            return m_texture_cell_corner_type_order[int_log2(TLR_BLR_C) + 29];

        // Impossible to reach? Not reached in huge grid
        std::cerr << "Not impossible to reach" << std::endl;

        return TEXTURE_CELL_NO_SIDES;
    }
};
