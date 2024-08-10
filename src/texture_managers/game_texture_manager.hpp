#pragma once

#include <iostream>
#include <memory>
#include <SDL.h>

#include "../game.hpp"
#include "../graphics/color.hpp"
#include "../graphics/font.hpp"
#include "../graphics/texture.hpp"

class GameTextureManager {
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

    enum CellSubtype {
        CELL_COVERED,
        CELL_COVERED_MINE,
        CELL_FLAGGED_MINE,
        CELL_TRIGGERED_MINE,
        CELL_FLAG,
    };

    enum TextureName {
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
        CLICK_TO_START,
    };

    using GameTexture = std::shared_ptr<Texture>;

private:
    static constexpr int CELL_TYPES = CELL_TRC + 1;
    static constexpr int CELL_SUBTYPES = CELL_FLAG + 1;
    static constexpr int CELL_TEXTURE_SIZE = 512;

    static constexpr auto CELL_MAP_IMAGE_PATH = "assets/textures/cell_map.png";
    static constexpr auto MINE_IMAGE_PATH = "assets/textures/mine.png";
    static constexpr auto FLAG_IMAGE_PATH = "assets/textures/flag.png";
    static constexpr auto H_GRID_LINE_IMAGE_PATH = "assets/textures/grid_line_horizontal.png";
    static constexpr auto V_GRID_LINE_IMAGE_PATH = "assets/textures/grid_line_vertical.png";
    static constexpr auto MOUSE_LEFT_ICON_PATH = "assets/textures/mouse_left.png";
    static constexpr auto MOUSE_RIGHT_ICON_PATH = "assets/textures/mouse_right.png";
    static constexpr auto BACK_BUTTON_IMAGE_PATH = "assets/textures/button_back.png";

    SDL_Renderer *m_renderer;
    const Game::Measurements &m_measurements;
    const int m_window_width;
    const int m_window_height;

    GameTexture m_h_grid_line_texture;
    GameTexture m_v_grid_line_texture;

    GameTexture m_cell_textures[CELL_SUBTYPES][CELL_TYPES];
    GameTexture m_cell_numbers_textures[8];

    GameTexture m_game_time_text_texture;
    GameTexture m_remaining_mines_text_texture;
    GameTexture m_remaining_mines_icon_texture;

    GameTexture m_mouse_left_icon_texture;
    GameTexture m_mouse_left_text_texture;
    GameTexture m_mouse_right_icon_texture;
    GameTexture m_mouse_right_text_texture;

    GameTexture m_back_button_texture;

    GameTexture m_click_to_start_texture;

    Font m_cell_number_font;

public:
    GameTextureManager(
        SDL_Renderer *renderer,
        const Game::Measurements &measurements,
        const int window_width,
        const int window_height
    ) : m_renderer(renderer),
        m_measurements(measurements),
        m_window_width(window_width),
        m_window_height(window_height),
        m_cell_number_font(Font::RUBIK_REGULAR, m_measurements.cell_size * 0.5) {
        init_grid_lines_textures();
        init_cell_numbers_textures();
        init_back_button_texture();
        init_remaining_mines_textures();
        init_game_time_texture();
        init_mouse_controls_textures();
        init_click_to_start_texture();

        const auto cell_map_texture = std::make_shared<Texture>(m_renderer, CELL_MAP_IMAGE_PATH);

        init_cell_textures_set(CELL_COVERED, cell_map_texture, Color::THEME);
        init_cell_textures_set(
            CELL_FLAG,
            cell_map_texture,
            Color::FLAGGED_CELL,
            FLAG_IMAGE_PATH,
            0.35,
            Color::FLAG
        );
        init_cell_textures_set(
            CELL_FLAGGED_MINE,
            cell_map_texture,
            Color::FLAGGED_CELL,
            MINE_IMAGE_PATH,
            0.5,
            Color::FLAG
        );
        init_cell_textures_set(
            CELL_COVERED_MINE,
            cell_map_texture,
            Color::THEME,
            MINE_IMAGE_PATH,
            0.5,
            Color::BACKGROUND
        );
        init_cell_textures_set(
            CELL_TRIGGERED_MINE,
            cell_map_texture,
            Color::TRIGGERED_CELL,
            MINE_IMAGE_PATH,
            0.5,
            Color::TRIGGERED_MINE
        );
    }

    ~GameTextureManager() = default;

    [[nodiscard]] GameTexture get(const CellSubtype subtype, const CellType type) const {
        return m_cell_textures[subtype][type];
    }

    [[nodiscard]] GameTexture get(const int cell_number) const {
        if (cell_number < 0 || cell_number > 7) {
            std::cerr << "Number must be between 0 and 7 inclusive." << std::endl;
            exit(1);
        }

        return m_cell_numbers_textures[cell_number];
    }

    [[nodiscard]] GameTexture get(const TextureName name) const {
        switch (name) {
            case H_GRID_LINE: return m_h_grid_line_texture;
            case V_GRID_LINE: return m_v_grid_line_texture;
            case GAME_TIME_TEXT: return m_game_time_text_texture;
            case REMAINING_MINES_TEXT: return m_remaining_mines_text_texture;
            case REMAINING_MINES_ICON: return m_remaining_mines_icon_texture;
            case MOUSE_LEFT_ICON: return m_mouse_left_icon_texture;
            case MOUSE_LEFT_TEXT: return m_mouse_left_text_texture;
            case MOUSE_RIGHT_ICON: return m_mouse_right_icon_texture;
            case MOUSE_RIGHT_TEXT: return m_mouse_right_text_texture;
            case BACK_BUTTON: return m_back_button_texture;
            case CLICK_TO_START: return m_click_to_start_texture;
        }
        __builtin_unreachable();
    }

private:
    void init_cell_textures_set(
        const CellSubtype cell_subtype,
        const GameTexture &cell_map_texture,
        const Color::Name cell_color,
        const char *image_path = nullptr,
        const float image_scale_respect_to_cell = 0,
        const Color::Name image_color = Color::BACKGROUND
    ) {
        const int cell_size = m_measurements.cell_size;
        const int grid_line_width = m_measurements.grid_line_width;
        const int image_size = cell_size * image_scale_respect_to_cell;
        const int image_offset = (grid_line_width + cell_size - image_size) / 2;

        const SDL_Rect texture_area = {0, 0, cell_size, cell_size};

        std::unique_ptr<Texture> image_texture;

        if (image_path != nullptr) {
            image_texture = std::make_unique<Texture>(
                m_renderer,
                image_path,
                SDL_Rect{image_offset, image_offset, image_size, image_size}
            );

            image_texture->set_color_mod(image_color);
        }

        cell_map_texture->set_color_mod(cell_color);

        for (int type = 0; type < CELL_TYPES; type++) {
            const auto cell_texture = std::make_shared<Texture>(m_renderer, texture_area);
            const Texture::ScopedRender scoped_render = cell_texture->set_as_render_target();

            const int map_index = type * CELL_TEXTURE_SIZE;
            const int map_x = map_index % cell_map_texture->get_w();
            const int map_y = map_index / cell_map_texture->get_h() * CELL_TEXTURE_SIZE;

            cell_map_texture->render({map_x, map_y, CELL_TEXTURE_SIZE, CELL_TEXTURE_SIZE});

            if (image_path != nullptr)
                image_texture->render();

            m_cell_textures[cell_subtype][type] = cell_texture;
        }

        cell_map_texture->set_color_mod(Color::WHITE);
    }

    void init_cell_numbers_textures() {
        const int cell_size = m_measurements.cell_size;
        const int grid_line_width = m_measurements.grid_line_width;

        for (int cell = 0; cell < 8; cell++) {
            char cell_text[2];
            snprintf(cell_text, 2, "%c", '0' + cell + 1);

            const auto cell_number_texture = std::make_shared<Texture>(
                m_renderer,
                m_cell_number_font.get_font(),
                cell_text,
                static_cast<Color::Name>(Color::GRID_1 + cell)
            );

            cell_number_texture->set_position(
                (grid_line_width + cell_size - cell_number_texture->get_w()) / 2,
                (grid_line_width + cell_size - cell_number_texture->get_h()) / 2
            );

            m_cell_numbers_textures[cell] = cell_number_texture;
        }
    }

    void init_grid_lines_textures() {
        const auto &[
            cell_size,
            grid_line_length,
            grid_line_width,
            grid_x_offset,
            grid_y_offset,
            grid_width,
            grid_height
        ] = m_measurements;

        const int grid_line_offset = (grid_line_width + cell_size - grid_line_length) / 2;
        const SDL_Color light_grey = Color::get(Color::LIGHT_GREY).get_rgb();

        m_h_grid_line_texture = std::make_shared<Texture>(
            m_renderer,
            H_GRID_LINE_IMAGE_PATH,
            SDL_Rect{grid_line_offset, 0, grid_line_length, grid_line_width}
        );
        m_h_grid_line_texture->set_color_mod(light_grey);

        m_v_grid_line_texture = std::make_shared<Texture>(
            m_renderer,
            V_GRID_LINE_IMAGE_PATH,
            SDL_Rect{0, grid_line_offset, grid_line_width, grid_line_length}
        );
        m_v_grid_line_texture->set_color_mod(light_grey);
    }

    void init_back_button_texture() {
        const int height = Font::get_shared(Font::PRIMARY)->get_size();
        m_back_button_texture = std::make_shared<Texture>(m_renderer, BACK_BUTTON_IMAGE_PATH);
        m_back_button_texture->set_position(30, 30);
        m_back_button_texture->set_height(height);
    }

    void init_remaining_mines_textures() {
        const int icon_size = Font::get_shared(Font::PRIMARY)->get_size();

        m_remaining_mines_icon_texture = std::make_shared<Texture>(m_renderer, MINE_IMAGE_PATH);
        m_remaining_mines_icon_texture->set_height(icon_size);

        m_remaining_mines_text_texture = std::make_shared<Texture>(
            m_renderer,
            Font::get_shared(Font::PRIMARY)->get_font(),
            "0",
            Color::WHITE
        );

        const int text_x_offset = m_remaining_mines_icon_texture->get_w() + 10;

        m_remaining_mines_icon_texture->set_position(
            (m_window_width - text_x_offset - m_remaining_mines_text_texture->get_w()) / 2,
            30
        );

        m_remaining_mines_text_texture->set_position(
            m_remaining_mines_icon_texture->get_x() + text_x_offset,
            m_remaining_mines_icon_texture->get_y() + (icon_size - m_remaining_mines_text_texture->get_h()) / 2
        );
    }

    void init_game_time_texture() {
        m_game_time_text_texture = std::make_shared<Texture>(
            m_renderer,
            Font::get_shared(Font::SECONDARY)->get_font(),
            "0",
            Color::LIGHTER_GREY
        );

        m_game_time_text_texture->set_position(
            (m_window_width - m_game_time_text_texture->get_w()) / 2,
            m_remaining_mines_icon_texture->get_y() + m_remaining_mines_icon_texture->get_h() + 10
        );
    }

    void init_mouse_controls_textures() {
        const int icon_height = Font::get_shared(Font::PRIMARY)->get_size() * 2;

        m_mouse_left_icon_texture = std::make_shared<Texture>(m_renderer, MOUSE_LEFT_ICON_PATH);
        m_mouse_left_icon_texture->set_position(30, m_window_height - icon_height * 2 - 30);
        m_mouse_left_icon_texture->set_height(icon_height);

        m_mouse_left_text_texture = std::make_shared<Texture>(
            m_renderer,
            Font::get_shared(Font::PRIMARY)->get_font(),
            "uncover",
            Color::WHITE
        );
        m_mouse_left_text_texture->set_position(
            m_mouse_left_icon_texture->get_x() + m_mouse_left_icon_texture->get_w() + 10,
            m_mouse_left_icon_texture->get_y() + (icon_height - m_mouse_left_text_texture->get_h()) / 2
        );

        m_mouse_right_icon_texture = std::make_shared<Texture>(m_renderer, MOUSE_RIGHT_ICON_PATH);
        m_mouse_right_icon_texture->set_position(30, m_window_height - icon_height - 20);
        m_mouse_right_icon_texture->set_height(icon_height);

        m_mouse_right_text_texture = std::make_shared<Texture>(
            m_renderer,
            Font::get_shared(Font::PRIMARY)->get_font(),
            "flag",
            Color::WHITE
        );
        m_mouse_right_text_texture->set_position(
            m_mouse_right_icon_texture->get_x() + m_mouse_right_icon_texture->get_w() + 10,
            m_mouse_right_icon_texture->get_y() + (icon_height - m_mouse_right_text_texture->get_h()) / 2
        );
    }

    void init_click_to_start_texture() {
        m_click_to_start_texture = std::make_shared<Texture>(
            m_renderer,
            Font::get_shared(Font::PRIMARY)->get_font(),
            "Click to start",
            Color::BACKGROUND
        );
        m_click_to_start_texture->set_position(
            (m_window_width - m_click_to_start_texture->get_w()) / 2,
            (m_window_height - m_click_to_start_texture->get_h()) / 2
        );
    }
};
