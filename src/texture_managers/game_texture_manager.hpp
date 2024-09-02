#pragma once

#include <iostream>
#include <memory>
#include <SDL.h>

#include "../core/game.hpp"
#include "../core/settings.hpp"
#include "../graphics/color.hpp"
#include "../graphics/font.hpp"
#include "../graphics/shape.hpp"
#include "../graphics/texture.hpp"
#include "../graphics/texture_bundle.hpp"

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
        ACTION_TOGGLE,
        ACTION_TOGGLE_MINE,
        ACTION_TOGGLE_MINE_SELECTED,
        ACTION_TOGGLE_FLAG,
        ACTION_TOGGLE_FLAG_SELECTED,
    };

    enum TextureBundleName {
        GAME_LOST,
        GAME_WON,
    };

    using GameTexture = std::shared_ptr<Texture>;
    using GameTextureBundle = std::shared_ptr<TextureBundle>;

private:
    struct CellTextureSetParameters {
        const Color::Name cell_color;
        const char *image_path;
        const float image_scale_respect_to_cell;
        const Color::Name image_color;
    };

    static constexpr int CELL_TYPES = CELL_TRC + 1;
    static constexpr int CELL_SUBTYPES = CELL_FLAG + 1;
    static constexpr int CELL_TEXTURE_SIZE = 512;

    static constexpr auto CELL_MAP_IMAGE_PATH = "assets/textures/cell_map.png";
    static constexpr auto MINE_IMAGE_PATH = "assets/textures/mine.png";
    static constexpr auto FLAG_IMAGE_PATH = "assets/textures/flag.png";

    // height:weight
    static constexpr double MOUSE_ICON_RATIO = 45.0 / 32;
    static constexpr double MOUSE_ICON_H_LINE_REL_POS = 23.0 / 45;
    static constexpr double ACTION_TOGGLE_THICKNESS_FACTOR = 1.0 / 32;
    static constexpr double BACK_BUTTON_THICKNESS_FACTOR = 1.0 / 8;
    static constexpr double MOUSE_ICON_THICKNESS_FACTOR = 1.0 / 8;

    static constexpr CellTextureSetParameters CELL_TEXTURE_SET_PARAMETERS[CELL_SUBTYPES] = {
        {Color::THEME, nullptr, 0, Color::BACKGROUND},
        {Color::THEME, MINE_IMAGE_PATH, 0.5, Color::BACKGROUND},
        {Color::FLAGGED_CELL, MINE_IMAGE_PATH, 0.5, Color::FLAG},
        {Color::TRIGGERED_CELL, MINE_IMAGE_PATH, 0.5, Color::TRIGGERED_MINE},
        {Color::FLAGGED_CELL, FLAG_IMAGE_PATH, 0.35, Color::FLAG},
    };

    SDL_Renderer *m_renderer;
    const Game::Measurements &m_measurements;
    const int m_window_width;
    const int m_window_height;
    const int m_window_padding;

    GameTexture m_h_grid_line_texture;
    GameTexture m_v_grid_line_texture;

    GameTexture m_cell_textures[CELL_SUBTYPES][CELL_TYPES];
    GameTexture m_cell_numbers_textures[8];

    GameTexture m_game_time_text_texture;
    GameTexture m_remaining_mines_text_texture;
    GameTexture m_remaining_mines_icon_texture;

    GameTexture m_action_toggle_texture;
    GameTexture m_action_toggle_mine_texture;
    GameTexture m_action_toggle_mine_selected_texture;
    GameTexture m_action_toggle_flag_texture;
    GameTexture m_action_toggle_flag_selected_texture;

    GameTexture m_mouse_left_icon_texture;
    GameTexture m_mouse_left_text_texture;
    GameTexture m_mouse_right_icon_texture;
    GameTexture m_mouse_right_text_texture;

    GameTexture m_back_button_texture;
    GameTexture m_click_to_start_texture;

    GameTextureBundle m_game_lost_texture_bundle;
    GameTextureBundle m_game_won_texture_bundle;

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
        m_window_padding(window_height * 0.025) {
        make_grid_lines_textures();
        make_cell_numbers_textures();
        make_back_button_texture();
        make_remaining_mines_textures();
        make_game_time_texture();
        make_click_to_start_texture();
        make_game_lost_texture_bundle();
        make_game_won_texture_bundle();

        if (Settings::is_on(Settings::SHOW_CONTROLS))
            make_mouse_controls_textures();

        if (Settings::is_on(Settings::SINGLE_CLICK_CONTROLS))
            make_action_toggle_textures();

        const auto cell_map_texture = std::make_shared<Texture>(m_renderer, CELL_MAP_IMAGE_PATH);

        for (int cell_subtype = 0; cell_subtype < CELL_SUBTYPES; cell_subtype++) {
            const auto &[
                cell_color,
                image_path,
                image_scale_respect_to_cell,
                image_color
            ] = CELL_TEXTURE_SET_PARAMETERS[cell_subtype];

            make_cell_textures_set(
                static_cast<CellSubtype>(cell_subtype),
                cell_map_texture,
                cell_color,
                image_path,
                image_scale_respect_to_cell,
                image_color
            );
        }
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
            case ACTION_TOGGLE: return m_action_toggle_texture;
            case ACTION_TOGGLE_MINE: return m_action_toggle_mine_texture;
            case ACTION_TOGGLE_MINE_SELECTED: return m_action_toggle_mine_selected_texture;
            case ACTION_TOGGLE_FLAG: return m_action_toggle_flag_texture;
            case ACTION_TOGGLE_FLAG_SELECTED: return m_action_toggle_flag_selected_texture;
        }
        __builtin_unreachable();
    }

    [[nodiscard]] GameTextureBundle get(const TextureBundleName name) const {
        switch (name) {
            case GAME_LOST: return m_game_lost_texture_bundle;
            case GAME_WON: return m_game_won_texture_bundle;
        }
        __builtin_unreachable();
    }

private:
    void make_cell_textures_set(
        const CellSubtype cell_subtype,
        const GameTexture &cell_map_texture,
        const Color::Name cell_color,
        const char *image_path,
        const float image_scale_respect_to_cell,
        const Color::Name image_color
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

            image_texture->set_color(image_color);
        }

        cell_map_texture->set_color(cell_color);

        for (int type = 0; type < CELL_TYPES; type++) {
            const auto cell_texture = std::make_shared<Texture>(m_renderer, texture_area);
            const Texture::ScopedRender scoped_render = cell_texture->set_as_render_target();

            const int map_index = type * CELL_TEXTURE_SIZE;
            const int map_x = map_index % cell_map_texture->get_w();
            const int map_y = map_index / cell_map_texture->get_h() * CELL_TEXTURE_SIZE;

            cell_map_texture->render_from(map_x, map_y, CELL_TEXTURE_SIZE, CELL_TEXTURE_SIZE);

            if (image_path != nullptr)
                image_texture->render();

            m_cell_textures[cell_subtype][type] = cell_texture;
        }

        cell_map_texture->set_color(Color::WHITE);
    }

    void make_cell_numbers_textures() {
        const int cell_size = m_measurements.cell_size;
        const int grid_line_width = m_measurements.grid_line_width;

        const Font cell_number_font(Font::RUBIK_REGULAR, m_measurements.cell_size * 0.5);

        for (int cell = 0; cell < 8; cell++) {
            const char cell_text[2] = {static_cast<char>('0' + cell + 1), 0};

            const auto cell_number_texture = std::make_shared<Texture>(
                m_renderer,
                cell_number_font.get_raw(),
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

    void make_grid_lines_textures() {
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
        const float thickness_2 = grid_line_width / 2.0f;

        m_h_grid_line_texture = std::make_shared<Texture>(
            m_renderer,
            SDL_Rect{grid_line_offset, -static_cast<int>(thickness_2), grid_line_length, grid_line_width}
        );

        const Texture::ScopedRender h_grid_line_renderer = m_h_grid_line_texture->set_as_render_target();

        Shape::rounded_line(
            m_renderer,
            grid_line_width,
            thickness_2,
            grid_line_length - grid_line_width,
            thickness_2,
            grid_line_width,
            Color::GREY
        );

        h_grid_line_renderer.release();

        m_v_grid_line_texture = std::make_shared<Texture>(
            m_renderer,
            SDL_Rect{-static_cast<int>(thickness_2), grid_line_offset, grid_line_width, grid_line_length}
        );

        const Texture::ScopedRender v_grid_line_renderer = m_v_grid_line_texture->set_as_render_target();

        Shape::rounded_line(
            m_renderer,
            thickness_2,
            grid_line_width,
            thickness_2,
            grid_line_length - grid_line_width,
            grid_line_width,
            Color::GREY
        );
    }

    void make_back_button_texture() {
        const int size = Font::get_shared(Font::PRIMARY)->get_size();
        const double thickness = size * BACK_BUTTON_THICKNESS_FACTOR;

        m_back_button_texture = std::make_shared<Texture>(
            m_renderer,
            SDL_Rect{m_window_padding, m_window_padding, size, size}
        );

        const Texture::ScopedRender scoped_render = m_back_button_texture->set_as_render_target();

        Shape::rounded_line(
            m_renderer,
            thickness,
            (size + thickness) / 2.0,
            (size - thickness) / 2.0,
            size - thickness,
            thickness,
            Color::WHITE
        );
        Shape::rounded_line(
            m_renderer,
            thickness,
            (size - thickness) / 2.0,
            (size - thickness) / 2.0,
            thickness,
            thickness,
            Color::WHITE
        );
        Shape::rounded_line(
            m_renderer,
            thickness,
            size / 2.0,
            size - thickness,
            size / 2.0,
            thickness,
            Color::WHITE
        );
    }

    void make_remaining_mines_textures() {
        const int icon_size = Font::get_shared(Font::PRIMARY)->get_size();

        m_remaining_mines_icon_texture = std::make_shared<Texture>(m_renderer, MINE_IMAGE_PATH);
        m_remaining_mines_icon_texture->set_height(icon_size);

        m_remaining_mines_text_texture = std::make_shared<Texture>(
            m_renderer,
            Font::get_shared(Font::PRIMARY)->get_raw(),
            "0",
            Color::WHITE
        );

        const int text_x_offset = icon_size + 10;

        m_remaining_mines_icon_texture->set_position(
            (m_window_width - text_x_offset - m_remaining_mines_text_texture->get_w()) / 2,
            m_window_padding
        );

        m_remaining_mines_text_texture->set_position(
            m_remaining_mines_icon_texture->get_x() + text_x_offset,
            m_remaining_mines_icon_texture->get_y() + (icon_size - m_remaining_mines_text_texture->get_h()) / 2
        );
    }

    void make_game_time_texture() {
        m_game_time_text_texture = std::make_shared<Texture>(
            m_renderer,
            Font::get_shared(Font::SECONDARY)->get_raw(),
            "0",
            Color::LIGHTER_GREY
        );

        m_game_time_text_texture->set_position(
            (m_window_width - m_game_time_text_texture->get_w()) / 2,
            m_remaining_mines_icon_texture->get_y() + m_remaining_mines_icon_texture->get_h() + m_window_padding / 2
        );
    }

    void make_mouse_controls_textures() {
        const bool swapped_controls = Settings::is_on(Settings::SWAP_CONTROLS);
        const bool single_click_controls = Settings::is_on(Settings::SINGLE_CLICK_CONTROLS);
        const int icon_height = Font::get_shared(Font::PRIMARY)->get_size() * 1.25;
        const double icon_width = icon_height / MOUSE_ICON_RATIO;
        const int icon_x = m_window_width - icon_width - m_window_padding;
        const int icon_thickness = icon_width * MOUSE_ICON_THICKNESS_FACTOR;
        const int text_x_offset = icon_x - m_window_padding / 2;
        const int icon_h_line_y = icon_width * MOUSE_ICON_H_LINE_REL_POS;

        m_mouse_left_icon_texture = std::make_shared<Texture>(
            m_renderer,
            SDL_Rect{icon_x, m_window_padding, static_cast<int>(icon_width), icon_height}
        );
        render_mouse_icon(
            m_mouse_left_icon_texture,
            icon_width,
            icon_height,
            icon_thickness,
            icon_h_line_y,
            !swapped_controls
        );

        m_mouse_left_text_texture = std::make_shared<Texture>(
            m_renderer,
            Font::get_shared(Font::PRIMARY)->get_raw(),
            single_click_controls ? "action" : "dig",
            Color::WHITE
        );
        m_mouse_left_text_texture->set_position(
            text_x_offset - m_mouse_left_text_texture->get_w(),
            m_mouse_left_icon_texture->get_y() + (icon_height - m_mouse_left_text_texture->get_h()) / 2
        );

        if (single_click_controls)
            return;

        m_mouse_right_icon_texture = std::make_shared<Texture>(
            m_renderer,
            SDL_Rect{
                icon_x,
                static_cast<int>(icon_height + m_window_padding * 1.5),
                static_cast<int>(icon_width),
                icon_height
            }
        );
        render_mouse_icon(
            m_mouse_right_icon_texture,
            icon_width,
            icon_height,
            icon_thickness,
            icon_h_line_y,
            swapped_controls
        );

        m_mouse_right_text_texture = std::make_shared<Texture>(
            m_renderer,
            Font::get_shared(Font::PRIMARY)->get_raw(),
            "flag",
            Color::WHITE
        );
        m_mouse_right_text_texture->set_position(
            text_x_offset - m_mouse_right_text_texture->get_w(),
            m_mouse_right_icon_texture->get_y() + (icon_height - m_mouse_right_text_texture->get_h()) / 2
        );
    }

    void make_action_toggle_textures() {
        const int action_toggle_width = m_window_width * 0.04;
        const int action_toggle_height = action_toggle_width * 2;
        const int action_toggle_x = m_window_width - action_toggle_width - m_window_padding;
        const int action_toggle_y = m_window_height - action_toggle_height - m_window_padding;

        m_action_toggle_texture = std::make_shared<Texture>(
            m_renderer,
            SDL_Rect{action_toggle_x, action_toggle_y, action_toggle_width, action_toggle_height}
        );

        const Texture::ScopedRender action_toggle_texture_renderer = m_action_toggle_texture->set_as_render_target();

        Shape::rounded_rectangle(
            m_renderer,
            {0, 0, action_toggle_width, action_toggle_height},
            action_toggle_width * ACTION_TOGGLE_THICKNESS_FACTOR,
            action_toggle_width / 2.0f,
            Color::LIGHT_GREY
        );

        action_toggle_texture_renderer.release();

        const int toggle_size = action_toggle_width * 0.78;
        const int toggle_padding = (action_toggle_width - toggle_size) / 2;
        const int toggle_x = action_toggle_x + toggle_padding;

        make_action_toggle_button_textures(
            m_action_toggle_flag_texture,
            m_action_toggle_flag_selected_texture,
            FLAG_IMAGE_PATH,
            0.45,
            toggle_size,
            toggle_x,
            action_toggle_y + toggle_padding
        );

        make_action_toggle_button_textures(
            m_action_toggle_mine_texture,
            m_action_toggle_mine_selected_texture,
            MINE_IMAGE_PATH,
            0.6,
            toggle_size,
            toggle_x,
            action_toggle_y + action_toggle_height - toggle_size - toggle_padding
        );
    }

    void make_action_toggle_button_textures(
        GameTexture &toggle_texture,
        GameTexture &toggle_selected_texture,
        const char *image_path,
        const float image_scale_respect_to_toggle,
        const int size,
        const int x,
        const int y
    ) {
        Texture image_texture(m_renderer, image_path);
        image_texture.set_height(size * image_scale_respect_to_toggle);
        image_texture.set_position((size - image_texture.get_w()) / 2, (size - image_texture.get_h()) / 2);

        toggle_texture = std::make_shared<Texture>(m_renderer, SDL_Rect{x, y, size, size});
        const Texture::ScopedRender toggle_scoped_render = toggle_texture->set_as_render_target();

        image_texture.set_color(Color::WHITE);
        image_texture.render();
        toggle_scoped_render.release();

        toggle_selected_texture = std::make_shared<Texture>(m_renderer, SDL_Rect{x, y, size, size});
        const Texture::ScopedRender toggle_selected_scoped_render = toggle_selected_texture->set_as_render_target();

        Shape::circle(m_renderer, 0, 0, size / 2.0, Color::THEME);

        image_texture.set_color(Color::BACKGROUND);
        image_texture.render();
        toggle_selected_scoped_render.release();
    }

    void make_click_to_start_texture() {
        m_click_to_start_texture = std::make_shared<Texture>(
            m_renderer,
            Font::get_shared(Font::PRIMARY)->get_raw(),
            "Click to start",
            Color::BACKGROUND
        );
        m_click_to_start_texture->set_position(
            (m_window_width - m_click_to_start_texture->get_w()) / 2,
            (m_window_height - m_click_to_start_texture->get_h()) / 2
        );
    }

    void make_game_lost_texture_bundle() {
        TextureBundle bundle;

        const int box_width = m_window_width * 0.4;
        const int box_height = box_width * 3 / 4;
        const int box_x = (m_window_width - box_width) / 2;
        const int box_y = (m_window_height - box_height) / 2;
        const float box_thickness = box_width * 0.002;
        const float box_radius = box_width * 0.05;

        const auto background_texture = std::make_shared<Texture>(
            m_renderer,
            SDL_Rect{0, 0, m_window_width, m_window_height}
        );
        bundle.add(background_texture);

        const Texture::ScopedRender background_renderer = background_texture->set_as_render_target();

        Shape::filled_rectangle(m_renderer, background_texture->get_area(), {0, 0, 0, 128});
        Shape::filled_rounded_rectangle(
            m_renderer,
            {box_x, box_y, box_width, box_height},
            box_thickness,
            box_radius,
            Color::BACKGROUND,
            Color::LIGHTER_GREY
        );

        background_renderer.release();

        m_game_lost_texture_bundle = std::make_shared<TextureBundle>(bundle);
    }

    void make_game_won_texture_bundle() {}

    void render_mouse_icon(
        const GameTexture &icon_texture,
        const double width,
        const int height,
        const double thickness,
        const int h_line_y,
        const bool is_left
    ) const {
        const Texture::ScopedRender scoped_render = icon_texture->set_as_render_target();

        Shape::rounded_rectangle(
            m_renderer,
            {0, 0, static_cast<int>(width), height},
            thickness,
            (width - thickness) / 2,
            Color::WHITE
        );
        Shape::filled_rectangle(
            m_renderer,
            {0, h_line_y, static_cast<int>(width), static_cast<int>(thickness)},
            Color::WHITE
        );
        Shape::circle_sector(
            m_renderer,
            0,
            0,
            width / 2,
            is_left ? 180 : 270,
            is_left ? 270 : 0,
            Color::WHITE
        );
    }
};
