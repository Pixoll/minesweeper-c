#pragma once

#include <iostream>
#include <memory>
#include <SDL.h>

#include "../core/game.hpp"
#include "../graphics/colors.hpp"
#include "../graphics/fonts.hpp"
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

public:
    GameTextureManager(SDL_Renderer *renderer, const Game::Measurements &measurements, const int window_height) {
        init_game_fonts(measurements.cell_size);

        init_grid_lines_textures(renderer, measurements);
        init_cell_numbers_textures(renderer, measurements);
        init_back_button_texture(renderer);
        init_remaining_mines_textures(renderer);
        init_game_time_texture(renderer);
        init_mouse_controls_textures(renderer, window_height);

        const auto cell_map_texture = std::make_shared<Texture>(renderer, CELL_MAP_IMAGE_PATH);

        init_cell_textures_set(
            renderer,
            measurements,
            CELL_COVERED,
            cell_map_texture,
            nullptr,
            0,
            Color::BACKGROUND,
            Color::THEME
        );
        init_cell_textures_set(
            renderer,
            measurements,
            CELL_FLAG,
            cell_map_texture,
            FLAG_IMAGE_PATH,
            0.35,
            Color::FLAGGED_CELL,
            Color::FLAGGED_CELL_BG
        );
        init_cell_textures_set(
            renderer,
            measurements,
            CELL_FLAGGED_MINE,
            cell_map_texture,
            MINE_IMAGE_PATH,
            0.5,
            Color::FLAGGED_CELL,
            Color::FLAGGED_CELL_BG
        );
        init_cell_textures_set(
            renderer,
            measurements,
            CELL_COVERED_MINE,
            cell_map_texture,
            MINE_IMAGE_PATH,
            0.5,
            Color::BACKGROUND,
            Color::THEME
        );
        init_cell_textures_set(
            renderer,
            measurements,
            CELL_TRIGGERED_MINE,
            cell_map_texture,
            MINE_IMAGE_PATH,
            0.5,
            Color::TRIGGERED_MINE,
            Color::TRIGGERED_MINE_BG
        );
    }

    ~GameTextureManager() {
        free_game_fonts();
    }

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

    // ReSharper disable once CppNotAllPathsReturnValue
    [[nodiscard]] GameTexture get(const Name name) const {
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
        }
        __builtin_unreachable();
    }

private:
    void init_cell_textures_set(
        SDL_Renderer *renderer,
        const Game::Measurements &measurements,
        const CellSubtype cell_subtype,
        const GameTexture &cell_map_texture,
        const char *image_path,
        const float image_scale_respect_to_cell,
        const Color::Name image_color,
        const Color::Name cell_color
    ) {
        const int cell_size = measurements.cell_size;
        const int cell_offset = measurements.cell_offset;
        const int grid_line_width = measurements.grid_line_width;
        const int image_size = cell_size * image_scale_respect_to_cell;
        const int image_offset = (grid_line_width + cell_size - image_size) / 2 - cell_offset;

        const SDL_Rect texture_area = {cell_offset, cell_offset, cell_size, cell_size};

        Texture image_texture{};

        if (image_path != nullptr) {
            image_texture = {
                renderer,
                image_path,
                {image_offset, image_offset, image_size, image_size},
            };

            image_texture.set_color_mod(image_color);
        }

        cell_map_texture->set_color_mod(cell_color);

        for (int type = 0; type < CELL_TYPES; type++) {
            const auto cell_texture = std::make_shared<Texture>(renderer, texture_area);
            const Texture::ScopedRender scoped_render = cell_texture->set_as_render_target();

            const int map_index = type * CELL_TEXTURE_SIZE;
            const int map_x = map_index % cell_map_texture->get_w();
            const int map_y = map_index / cell_map_texture->get_h() * CELL_TEXTURE_SIZE;

            cell_map_texture->render({map_x, map_y, CELL_TEXTURE_SIZE, CELL_TEXTURE_SIZE});

            if (image_path != nullptr)
                image_texture.render();

            m_cell_textures[cell_subtype][type] = cell_texture;
        }

        cell_map_texture->set_color_mod(Color::WHITE);
    }

    void init_cell_numbers_textures(SDL_Renderer *renderer, const Game::Measurements &measurements) {
        const int cell_size = measurements.cell_size;
        const int grid_line_width = measurements.grid_line_width;

        for (int cell = Game::CELL_1; cell <= Game::CELL_8; cell++) {
            char cell_text[2];
            snprintf(cell_text, 2, "%c", '0' + cell - Game::CELL_0);

            const auto cell_number_texture = std::make_shared<Texture>(
                renderer,
                Font::CELL_NUMBER,
                cell_text,
                static_cast<Color::Name>(Color::GRID_1 + cell - Game::CELL_1)
            );

            cell_number_texture->set_position(
                (grid_line_width + cell_size - cell_number_texture->get_w()) / 2,
                (grid_line_width + cell_size - cell_number_texture->get_h()) / 2
            );

            m_cell_numbers_textures[cell - Game::CELL_1] = cell_number_texture;
        }
    }

    void init_grid_lines_textures(SDL_Renderer *renderer, const Game::Measurements &measurements) {
        const auto &[
            cell_size,
            cell_offset,
            grid_line_length,
            grid_line_width,
            grid_x_offset,
            grid_y_offset,
            grid_width,
            grid_height
        ] = measurements;

        const int grid_line_offset = (grid_line_width + cell_size - grid_line_length) / 2;
        const SDL_Color light_grey = Color::get(Color::LIGHT_GREY).rgb;

        m_h_grid_line_texture = std::make_shared<Texture>(
            renderer,
            H_GRID_LINE_IMAGE_PATH,
            SDL_Rect{grid_line_offset, 0, grid_line_length, grid_line_width}
        );
        m_h_grid_line_texture->set_color_mod(light_grey);

        m_v_grid_line_texture = std::make_shared<Texture>(
            renderer,
            V_GRID_LINE_IMAGE_PATH,
            SDL_Rect{0, grid_line_offset, grid_line_width, grid_line_length}
        );
        m_v_grid_line_texture->set_color_mod(light_grey);
    }

    void init_back_button_texture(SDL_Renderer *renderer) {
        const int height = get_font(Font::PRIMARY).size * 1.25;
        m_back_button_texture = std::make_shared<Texture>(renderer, BACK_BUTTON_IMAGE_PATH);
        m_back_button_texture->set_position(20, 20);
        m_back_button_texture->set_height(height);
    }

    void init_remaining_mines_textures(SDL_Renderer *renderer) {
        const int size = get_font(Font::PRIMARY).size;
        m_remaining_mines_icon_texture = std::make_shared<Texture>(
            renderer,
            MINE_IMAGE_PATH,
            SDL_Rect{20, m_back_button_texture->get_y() + m_back_button_texture->get_h() + 20, size, size}
        );

        m_remaining_mines_text_texture = std::make_shared<Texture>(renderer, Font::PRIMARY, "0", Color::WHITE);
        m_remaining_mines_text_texture->set_position(
            m_remaining_mines_icon_texture->get_x() + m_remaining_mines_icon_texture->get_w() + 10,
            m_remaining_mines_icon_texture->get_y()
            + (m_remaining_mines_icon_texture->get_h() - m_remaining_mines_text_texture->get_h()) / 2
        );
    }

    void init_game_time_texture(SDL_Renderer *renderer) {
        m_game_time_text_texture = std::make_shared<Texture>(renderer, Font::SECONDARY, "0", Color::LIGHTER_GREY);
        m_game_time_text_texture->set_position(
            20,
            m_remaining_mines_icon_texture->get_y() + m_remaining_mines_icon_texture->get_h() + 10
        );
    }

    void init_mouse_controls_textures(SDL_Renderer *renderer, const int window_height) {
        const int icon_height = get_font(Font::PRIMARY).size * 2;

        m_mouse_left_icon_texture = std::make_shared<Texture>(renderer, MOUSE_LEFT_ICON_PATH);
        m_mouse_left_icon_texture->set_position(20, window_height - icon_height * 2 - 30);
        m_mouse_left_icon_texture->set_height(icon_height);

        m_mouse_left_text_texture = std::make_shared<Texture>(renderer, Font::PRIMARY, "uncover", Color::WHITE);
        m_mouse_left_text_texture->set_position(
            m_mouse_left_icon_texture->get_x() + m_mouse_left_icon_texture->get_w() + 10,
            m_mouse_left_icon_texture->get_y() + (icon_height - m_mouse_left_text_texture->get_h()) / 2
        );

        m_mouse_right_icon_texture = std::make_shared<Texture>(renderer, MOUSE_RIGHT_ICON_PATH);
        m_mouse_right_icon_texture->set_position(20, window_height - icon_height - 20);
        m_mouse_right_icon_texture->set_height(icon_height);

        m_mouse_right_text_texture = std::make_shared<Texture>(renderer, Font::PRIMARY, "flag", Color::WHITE);
        m_mouse_right_text_texture->set_position(
            m_mouse_right_icon_texture->get_x() + m_mouse_right_icon_texture->get_w() + 10,
            m_mouse_right_icon_texture->get_y() + (icon_height - m_mouse_right_text_texture->get_h()) / 2
        );
    }
};
