#pragma once

#include <ctime>
#include <iostream>
#include <string>

#include "screen.hpp"
#include "../core/game.hpp"
#include "../texture_managers/game_texture_manager.hpp"

class Engine;
class MainMenuScreen;

class GameScreen final : virtual public Screen {
    using GameTexture = GameTextureManager::GameTexture;
    using TextureName = GameTextureManager::TextureName;

    Engine *m_engine;
    int m_window_width;
    int m_window_height;
    Game m_game;
    GameTextureManager m_texture_manager;
    bool m_started_game;
    time_t m_last_game_time_rendered = 0;
    int m_remaining_mines = 0;

    SDL_Cursor *const m_arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    SDL_Cursor *const m_hand_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

public:
    explicit GameScreen(Engine *engine, const Game::Difficulty difficulty) :
        m_engine(engine),
        m_window_width(engine->get_window_width()),
        m_window_height(engine->get_window_height()),
        m_game(difficulty, engine->get_window_width(), engine->get_window_height()),
        m_texture_manager(engine->get_renderer(), m_game.get_measurements(), m_window_width, m_window_height),
        m_started_game(false) {}

    explicit GameScreen(Engine *engine, const Game &game) :
        m_engine(engine),
        m_window_width(engine->get_window_width()),
        m_window_height(engine->get_window_height()),
        m_game(game),
        m_texture_manager(engine->get_renderer(), m_game.get_measurements(), m_window_width, m_window_height),
        m_started_game(true) {}

    ~GameScreen() override = default;

    void run_logic(const SDL_Event &event) override {
        SDL_Point cursor_pos;
        SDL_GetMouseState(&cursor_pos.x, &cursor_pos.y);

        const bool cursor_in_back_button = m_texture_manager.get(TextureName::BACK_BUTTON)->contains(cursor_pos);
        SDL_SetCursor(cursor_in_back_button ? m_hand_cursor : m_arrow_cursor);

        if (event.type == SDL_QUIT && m_game.has_started() && !m_game.is_over()) {
            m_game.save();
            return;
        }

        if (event.type != SDL_MOUSEBUTTONDOWN)
            return;

        const auto [x, y, inside_cell] = m_game.calculate_grid_cell(cursor_pos.x, cursor_pos.y);

        if (!inside_cell) {
            if (cursor_in_back_button && event.button.button == SDL_BUTTON_LEFT) {
                if (m_game.has_started() && !m_game.is_over())
                    m_game.save();

                m_engine->set_screen<MainMenuScreen>(m_engine);
            }

            return;
        }

        if (m_game.is_over())
            return;

        switch (event.button.button) {
            case SDL_BUTTON_LEFT: {
                if (!m_started_game) {
                    m_game.place_grid_mines(x, y);
                    m_started_game = true;
                }

                m_game.reveal_cell(x, y);
                break;
            }

            case SDL_BUTTON_RIGHT: {
                if (!m_started_game)
                    break;

                m_game.toggle_cell_flag(x, y);
                break;
            }

            default:
                break;
        }
    }

    void render() override {
        render_grid();
        render_remaining_mines();
        if (m_game.has_started())
            render_game_time();

        if (Settings::is_on(Settings::SHOW_CONTROLS)) {
            m_texture_manager.get(TextureName::MOUSE_LEFT_ICON)->render();
            m_texture_manager.get(TextureName::MOUSE_LEFT_TEXT)->render();
            m_texture_manager.get(TextureName::MOUSE_RIGHT_ICON)->render();
            m_texture_manager.get(TextureName::MOUSE_RIGHT_TEXT)->render();
        }

        m_texture_manager.get(TextureName::BACK_BUTTON)->render();

        if (!m_started_game)
            m_texture_manager.get(TextureName::CLICK_TO_START)->render();
    }

private:
    void render_grid() const {
        const int rows = m_game.get_rows();
        const int columns = m_game.get_columns();

        const Game::Measurements &measurements = m_game.get_measurements();
        const int cell_size = measurements.cell_size;
        const int grid_x_offset = measurements.grid_x_offset;
        const int grid_y_offset = measurements.grid_y_offset;

        const GameTexture h_grid_line_texture = m_texture_manager.get(TextureName::H_GRID_LINE);
        const GameTexture v_grid_line_texture = m_texture_manager.get(TextureName::V_GRID_LINE);

        for (int i = 0; i < columns; i++) {
            const int x = grid_x_offset + cell_size * i;

            for (int j = 0; j < rows; j++) {
                const int y = grid_y_offset + cell_size * j;
                const Game::GridCell cell = m_game.get_grid_cell(i, j);

                // Render cells
                if (cell.type != Game::CELL_0 || !cell.revealed) {
                    const GameTextureManager::CellType cell_type = get_cell_type(i, j, cell.flagged, cell.revealed);
                    const GameTexture cell_texture = get_grid_cell_texture(cell, cell_type);

                    cell_texture->render_moved(x, y);
                }

                // Render grid
                if (j != rows - 1 && (cell.revealed || m_game.get_grid_cell(i, j + 1).revealed))
                    h_grid_line_texture->render_moved(x, y + cell_size);

                if (i != columns - 1 && (cell.revealed || m_game.get_grid_cell(i + 1, j).revealed))
                    v_grid_line_texture->render_moved(x + cell_size, y);
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

    void render_remaining_mines() {
        const GameTexture remaining_mines_text_texture = m_texture_manager.get(TextureName::REMAINING_MINES_TEXT);
        const int current_remaining = m_game.get_remaining_mines();

        if (m_remaining_mines != current_remaining) {
            m_remaining_mines = current_remaining;

            remaining_mines_text_texture->update_text(std::to_string(current_remaining));

            const GameTexture remaining_mines_icon_texture = m_texture_manager.get(TextureName::REMAINING_MINES_ICON);
            const int text_x_offset = remaining_mines_icon_texture->get_w() + 10;

            remaining_mines_icon_texture->set_x(
                (m_window_width - text_x_offset - remaining_mines_text_texture->get_w()) / 2
            );
            remaining_mines_text_texture->set_x(remaining_mines_icon_texture->get_x() + text_x_offset);
        }

        m_texture_manager.get(TextureName::REMAINING_MINES_ICON)->render();
        remaining_mines_text_texture->render();
    }

    static std::string get_time_string(const int seconds) {
        using std::to_string;

        const int sec = seconds % 60;
        const int min = seconds / 60;

        std::string time_string;

        if (min != 0)
            time_string += to_string(min) + "M ";

        time_string += to_string(sec) + "S";

        return time_string;
    }

    void render_game_time() {
        const GameTexture game_time_text_texture = m_texture_manager.get(TextureName::GAME_TIME_TEXT);
        const time_t now = time(nullptr);

        if (m_last_game_time_rendered == 0 || (!m_game.is_over() && m_last_game_time_rendered < now)) {
            m_last_game_time_rendered = now;

            const std::string time_string = get_time_string(now - m_game.get_start_time());
            game_time_text_texture->update_text(time_string);
            game_time_text_texture->set_x((m_window_width - game_time_text_texture->get_w()) / 2);
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

    [[nodiscard]] bool cell_matches_type(const int x, const int y, const bool flagged) const {
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

        const bool T = y - 1 >= 0 && cell_matches_type(x, y - 1, flagged);
        const bool B = y + 1 <= rows - 1 && cell_matches_type(x, y + 1, flagged);
        const bool L = x - 1 >= 0 && cell_matches_type(x - 1, y, flagged);
        const bool R = x + 1 <= columns - 1 && cell_matches_type(x + 1, y, flagged);
        const bool TLC = T && L && cell_matches_type(x - 1, y - 1, flagged);
        const bool TRC = T && R && cell_matches_type(x + 1, y - 1, flagged);
        const bool BLC = B && L && cell_matches_type(x - 1, y + 1, flagged);
        const bool BRC = B && R && cell_matches_type(x + 1, y + 1, flagged);

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
