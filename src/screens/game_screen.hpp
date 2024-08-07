#pragma once

#include <ctime>
#include <iostream>
#include <string>

#include "screen.hpp"
#include "../core/game.hpp"
#include "../texture_managers/game_texture_manager.hpp"

class MainMenuScreen;
class Engine;

class GameScreen final : virtual public Screen {
    using GameTexture = GameTextureManager::GameTexture;

    Engine *m_engine;
    SDL_Renderer *m_renderer;
    int m_window_width;
    int m_window_height;
    Game m_game;
    GameTextureManager m_texture_manager;
    bool m_placed_mines;
    time_t m_last_game_time_drawn = 0;
    int m_remaining_mines = 0;

    SDL_Cursor *const m_arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    SDL_Cursor *const m_hand_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

public:
    explicit GameScreen(Engine *engine, const int rows, const int columns, const int mines_count) :
        m_engine(engine),
        m_renderer(engine->get_renderer()),
        m_window_width(engine->get_window_width()),
        m_window_height(engine->get_window_height()),
        m_game(rows, columns, mines_count, engine->get_window_width(), engine->get_window_height()),
        m_texture_manager(m_renderer, m_game.get_measurements(), m_window_height),
        m_placed_mines(false) {}

    explicit GameScreen(Engine *engine, const Game &game) :
        m_engine(engine),
        m_renderer(engine->get_renderer()),
        m_window_width(engine->get_window_width()),
        m_window_height(engine->get_window_height()),
        m_game(game),
        m_texture_manager(m_renderer, m_game.get_measurements(), m_window_height),
        m_placed_mines(true) {}

    ~GameScreen() override = default;

    void run_logic(const SDL_Event &event) override {
        int click_x, click_y;
        SDL_GetMouseState(&click_x, &click_y);

        const bool cursor_in_back_button = m_texture_manager.get(GameTextureManager::BACK_BUTTON)
                                                           ->contains(click_x, click_y);
        SDL_SetCursor(cursor_in_back_button ? m_hand_cursor : m_arrow_cursor);

        if (event.type == SDL_QUIT && m_game.get_start_time() != 0 && !m_game.is_over()) {
            m_game.save();
            return;
        }

        if (event.type != SDL_MOUSEBUTTONDOWN)
            return;

        const auto [x, y, inside_cell] = m_game.calculate_grid_cell(click_x, click_y);

        if (!inside_cell) {
            if (cursor_in_back_button && event.button.button == SDL_BUTTON_LEFT) {
                if (m_game.get_start_time() != 0 && !m_game.is_over())
                    m_game.save();

                m_engine->set_screen<MainMenuScreen>(m_engine);
                SDL_SetCursor(m_arrow_cursor);
            }

            return;
        }

        if (m_game.is_over())
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

        // game controls
        m_texture_manager.get(GameTextureManager::MOUSE_LEFT_ICON)->render();
        m_texture_manager.get(GameTextureManager::MOUSE_LEFT_TEXT)->render();
        m_texture_manager.get(GameTextureManager::MOUSE_RIGHT_ICON)->render();
        m_texture_manager.get(GameTextureManager::MOUSE_RIGHT_TEXT)->render();

        m_texture_manager.get(GameTextureManager::BACK_BUTTON)->render();

        SDL_RenderPresent(m_renderer);
    }

private:
    void draw_grid() const {
        const int rows = m_game.get_rows();
        const int columns = m_game.get_columns();

        const Game::Measurements &measurements = m_game.get_measurements();
        const int cell_size = measurements.cell_size;
        const int grid_x_offset = measurements.grid_x_offset;
        const int grid_y_offset = measurements.grid_y_offset;

        const GameTexture h_grid_line_texture = m_texture_manager.get(GameTextureManager::H_GRID_LINE);
        const GameTexture v_grid_line_texture = m_texture_manager.get(GameTextureManager::V_GRID_LINE);

        for (int i = 0; i < columns; i++) {
            const int x = grid_x_offset + cell_size * i;

            for (int j = 0; j < rows; j++) {
                const int y = grid_y_offset + cell_size * j;
                const Game::GridCell cell = m_game.get_grid_cell(i, j);

                // Draw cells
                if (cell.type != Game::CELL_0 || !cell.revealed) {
                    const GameTextureManager::CellType cell_type = get_cell_type(i, j, cell.flagged, cell.revealed);
                    const GameTexture cell_texture = get_grid_cell_texture(cell, cell_type);

                    cell_texture->render(cell_texture->get_x() + x, cell_texture->get_y() + y);
                }

                // Draw grid
                if (j != rows - 1 && (cell.revealed || m_game.get_grid_cell(i, j + 1).revealed)) {
                    h_grid_line_texture->render(
                        x + h_grid_line_texture->get_x(),
                        y + cell_size + h_grid_line_texture->get_y()
                    );
                }

                if (i != columns - 1 && (cell.revealed || m_game.get_grid_cell(i + 1, j).revealed)) {
                    v_grid_line_texture->render(
                        x + cell_size + v_grid_line_texture->get_x(),
                        y + v_grid_line_texture->get_y()
                    );
                }
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
        const GameTexture remaining_mines_text_texture = m_texture_manager.get(
            GameTextureManager::REMAINING_MINES_TEXT
        );
        const int current_remaining = m_game.get_total_mines() - m_game.get_flagged_mines();

        if (m_remaining_mines != current_remaining) {
            m_remaining_mines = current_remaining;

            char remaining_mines_string[int_length(current_remaining) + 1];
            itoa(current_remaining, remaining_mines_string, 10);

            remaining_mines_text_texture->update_text(remaining_mines_string);
        }

        m_texture_manager.get(GameTextureManager::REMAINING_MINES_ICON)->render();
        remaining_mines_text_texture->render();
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

        const GameTexture game_time_text_texture = m_texture_manager.get(GameTextureManager::GAME_TIME_TEXT);
        const time_t start_time = m_game.get_start_time();

        if (start_time == 0)
            return;

        const time_t now = time(nullptr);
        if (m_last_game_time_drawn == 0 || (!m_game.is_over() && m_last_game_time_drawn < now)) {
            m_last_game_time_drawn = now;

            const string time_string = get_time_string(now - start_time);
            game_time_text_texture->update_text(time_string.c_str());
        }

        game_time_text_texture->render();
    }

    [[nodiscard]] GameTexture get_grid_cell_texture(
        const Game::GridCell cell,
        const GameTextureManager::CellType type
    ) const {
        if (m_game.is_over() && !m_game.has_won() && cell.type == Game::CELL_MINE) {
            if (cell.flagged)
                return m_texture_manager.get(GameTextureManager::CELL_FLAGGED_MINE, type);

            if (cell.revealed)
                return m_texture_manager.get(GameTextureManager::CELL_TRIGGERED_MINE, type);

            return m_texture_manager.get(GameTextureManager::CELL_COVERED_MINE, type);
        }

        if (cell.flagged)
            return m_texture_manager.get(GameTextureManager::CELL_FLAG, type);

        if (!cell.revealed)
            return m_texture_manager.get(GameTextureManager::CELL_COVERED, type);

        return m_texture_manager.get(cell.type - Game::CELL_1);
    }

    [[nodiscard]] bool verify_cell(const int x, const int y, const bool flagged) const {
        const auto [type, cell_flagged, revealed] = m_game.get_grid_cell(x, y);
        return !revealed && flagged == cell_flagged;
    }

    static bool exclusively_has_corners(const int corners, const int required_corners) {
        const bool bits_outside_of_mask = corners & ~required_corners;
        const bool bits_in_mask = corners & required_corners;
        return !bits_outside_of_mask && bits_in_mask;
    }

    static int has_single_corner(const int x) {
        return x > 0 && !(x & x - 1);
    }

    static int obtain_single_corner(int x) {
        int log2 = 0;
        while (x >>= 1)
            log2++;

        return log2;
    }

    static constexpr GameTextureManager::CellType TEXTURE_CELL_SIDE_TYPE_ORDER[16] = {
        GameTextureManager::CELL_NO_SIDES,
        GameTextureManager::CELL_R,
        GameTextureManager::CELL_L,
        GameTextureManager::CELL_LR,
        GameTextureManager::CELL_B,
        GameTextureManager::CELL_BR,
        GameTextureManager::CELL_BL,
        GameTextureManager::CELL_BLR,
        GameTextureManager::CELL_T,
        GameTextureManager::CELL_TR,
        GameTextureManager::CELL_TL,
        GameTextureManager::CELL_TLR,
        GameTextureManager::CELL_BT,
        GameTextureManager::CELL_TBR,
        GameTextureManager::CELL_TBL,
        GameTextureManager::CELL_TBLR,
    };

    static constexpr GameTextureManager::CellType TEXTURE_CELL_CORNER_TYPE_ORDER[33] = {
        GameTextureManager::CELL_TBLR_BRC,
        GameTextureManager::CELL_TBLR_BLC,
        GameTextureManager::CELL_TBLR_BLCRC,
        GameTextureManager::CELL_TBLR_TRC,
        GameTextureManager::CELL_TBLR_TRC_BRC,
        GameTextureManager::CELL_TBLR_TRC_BLC,
        GameTextureManager::CELL_TBLR_TRC_BLCRC,
        GameTextureManager::CELL_TBLR_TLC,
        GameTextureManager::CELL_TBLR_TLC_BRC,
        GameTextureManager::CELL_TBLR_TLC_BLC,
        GameTextureManager::CELL_TBLR_TLC_BLCRC,
        GameTextureManager::CELL_TBLR_TLCRC,
        GameTextureManager::CELL_TBLR_TLCRC_BRC,
        GameTextureManager::CELL_TBLR_TLCRC_BLC,
        GameTextureManager::CELL_TBLR_TLCRC_BLCRC,

        GameTextureManager::CELL_BLR_BRC,
        GameTextureManager::CELL_BLR_BLC,
        GameTextureManager::CELL_BLR_BLCRC,
        GameTextureManager::CELL_TBR_BRC,
        GameTextureManager::CELL_TLR_TRC,
        GameTextureManager::CELL_TBL_BLC,
        GameTextureManager::CELL_TBR_TRC,
        GameTextureManager::CELL_TBR_TRC_BRC,
        GameTextureManager::CELL_TLR_TLC,
        GameTextureManager::CELL_NO_SIDES,
        GameTextureManager::CELL_NO_SIDES,
        GameTextureManager::CELL_TBL_TLC,
        GameTextureManager::CELL_TLR_TLCRC,
        GameTextureManager::CELL_TBL_TLC_BLC,

        GameTextureManager::CELL_BRC,
        GameTextureManager::CELL_BLC,
        GameTextureManager::CELL_TRC,
        GameTextureManager::CELL_TLC,
    };

    [[nodiscard]] GameTextureManager::CellType get_cell_type(
        const int x,
        const int y,
        const bool flagged,
        const bool revealed
    ) const {
        if (revealed)
            return GameTextureManager::CELL_NO_SIDES;

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
            return TEXTURE_CELL_SIDE_TYPE_ORDER[TBLR];

        if (TBLR == 0b1111)
            return TEXTURE_CELL_CORNER_TYPE_ORDER[TLR_BLR_C - 1];

        if (TBLR == 0b0111 && exclusively_has_corners(TLR_BLR_C, 0b0011))
            return TEXTURE_CELL_CORNER_TYPE_ORDER[TLR_BLR_C + 14];
        if (TBLR == 0b1011 && exclusively_has_corners(TLR_BLR_C, 0b1100))
            return TEXTURE_CELL_CORNER_TYPE_ORDER[TLR_BLR_C + 15];
        if (TBLR == 0b1101 && exclusively_has_corners(TLR_BLR_C, 0b0101))
            return TEXTURE_CELL_CORNER_TYPE_ORDER[TLR_BLR_C + 17];
        if (TBLR == 0b1110 && exclusively_has_corners(TLR_BLR_C, 0b1010))
            return TEXTURE_CELL_CORNER_TYPE_ORDER[TLR_BLR_C + 18];

        if (has_single_corner(TLR_BLR_C))
            return TEXTURE_CELL_CORNER_TYPE_ORDER[obtain_single_corner(TLR_BLR_C) + 29];

        // Impossible to reach? Not reached in huge grid
        std::cerr << "Not impossible to reach" << std::endl;

        return GameTextureManager::CELL_NO_SIDES;
    }
};
