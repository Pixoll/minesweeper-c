#include "game_textures.hpp"

#include <iostream>
#include <SDL.h>

#include "colors.hpp"
#include "fonts.hpp"
#include "texture.hpp"
#include "../core/game.hpp"

Texture h_grid_line_texture;
Texture v_grid_line_texture;

Texture cell_map_texture;
constexpr int cell_texture_size = 512;

Texture cell_textures[GameTexture::CELL_SUBTYPES][GameTexture::CELL_TYPES];
Texture cell_numbers_textures[8];

Texture game_time_text_texture;
Texture remaining_mines_text_texture;
Texture remaining_mines_icon_texture;

Texture mouse_left_icon_texture;
Texture mouse_left_text_texture;
Texture mouse_right_icon_texture;
Texture mouse_right_text_texture;

Texture back_button_texture;

const auto cell_map_path = "assets/textures/cell_map.png";
const auto mine_image_path = "assets/textures/mine.png";
const auto flag_image_path = "assets/textures/flag.png";

const auto grid_line_horizontal_image_path = "assets/textures/grid_line_horizontal.png";
const auto grid_line_vertical_image_path = "assets/textures/grid_line_vertical.png";

const auto mouse_left_icon_path = "assets/textures/mouse_left.png";
const auto mouse_right_icon_path = "assets/textures/mouse_right.png";

const auto back_button_path = "assets/textures/button_back.png";

void init_cell_map_texture(SDL_Renderer *renderer) {
    cell_map_texture = {renderer, cell_map_path};
}

void init_cell_textures_set(
    SDL_Renderer *renderer,
    const Game::Measurements &measurements,
    Texture textures[GameTexture::CELL_TYPES],
    const char *image_path,
    const float image_scale_wrt_cell,
    const Color::Name image_color,
    const Color::Name cell_color
) {
    const int cell_size = measurements.cell_size;
    const int cell_offset = measurements.cell_offset;
    const int grid_line_width = measurements.grid_line_width;
    const int image_size = cell_size * image_scale_wrt_cell;
    const int image_offset = (grid_line_width + cell_size - image_size) / 2 - cell_offset;

    const SDL_Rect texture_area = {cell_offset, cell_offset, cell_size, cell_size};

    Texture image_texture{};

    if (image_path != nullptr && image_scale_wrt_cell != 0) {
        image_texture = {
            renderer,
            image_path,
            {image_offset, image_offset, image_size, image_size},
        };

        image_texture.set_color_mod(image_color);
    }

    cell_map_texture.set_color_mod(cell_color);

    for (int type = 0; type < GameTexture::CELL_TYPES; type++) {
        textures[type] = {renderer, texture_area};

        const Texture::ScopedRender scoped_render = textures[type].set_as_render_target();

        const int map_index = type * cell_texture_size;
        const int map_x = map_index % cell_map_texture.get_w();
        const int map_y = map_index / cell_map_texture.get_h() * cell_texture_size;

        cell_map_texture.render({map_x, map_y, cell_texture_size, cell_texture_size});

        if (image_path != nullptr && image_scale_wrt_cell != 0)
            image_texture.render();
    }

    cell_map_texture.set_color_mod(Color::WHITE);
    image_texture.destroy();
}

void init_cell_numbers_textures(SDL_Renderer *renderer, const Game::Measurements &measurements) {
    const int cell_size = measurements.cell_size;
    const int grid_line_width = measurements.grid_line_width;

    for (int cell = Game::CELL_1; cell <= Game::CELL_8; cell++) {
        char cell_text[2];
        snprintf(cell_text, 2, "%c", '0' + cell - Game::CELL_0);

        cell_numbers_textures[cell - Game::CELL_1] = {
            renderer,
            Font::CELL_NUMBER,
            cell_text,
            static_cast<Color::Name>(Color::GRID_1 + cell - Game::CELL_1)
        };

        cell_numbers_textures[cell - Game::CELL_1].set_position(
            (grid_line_width + cell_size - cell_numbers_textures->get_w()) / 2,
            (grid_line_width + cell_size - cell_numbers_textures->get_h()) / 2
        );
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
    const SDL_Color light_grey = get_color(Color::LIGHT_GREY).rgb;

    h_grid_line_texture = {
        renderer,
        grid_line_horizontal_image_path,
        {grid_line_offset, 0, grid_line_length, grid_line_width},
    };
    h_grid_line_texture.set_color_mod(light_grey);

    v_grid_line_texture = {
        renderer,
        grid_line_vertical_image_path,
        {0, grid_line_offset, grid_line_width, grid_line_length},
    };
    v_grid_line_texture.set_color_mod(light_grey);
}

void init_back_button_texture(SDL_Renderer *renderer) {
    const int height = get_font(Font::PRIMARY).size * 1.25;
    back_button_texture = {renderer, back_button_path};
    back_button_texture.set_position(20, 20);
    back_button_texture.set_height(height);
}

void init_remaining_mines_textures(SDL_Renderer *renderer) {
    const int size = get_font(Font::PRIMARY).size;
    remaining_mines_icon_texture = {
        renderer,
        mine_image_path,
        {20, back_button_texture.get_y() + back_button_texture.get_h() + 20, size, size},
    };

    remaining_mines_text_texture = {renderer, Font::PRIMARY, "0", Color::WHITE};
    remaining_mines_text_texture.set_position(
        remaining_mines_icon_texture.get_x() + remaining_mines_icon_texture.get_w() + 10,
        remaining_mines_icon_texture.get_y()
        + (remaining_mines_icon_texture.get_h() - remaining_mines_text_texture.get_h()) / 2
    );
}

void init_game_time_texture(SDL_Renderer *renderer) {
    game_time_text_texture = {renderer, Font::SECONDARY, "0", Color::LIGHTER_GREY};
    game_time_text_texture.set_position(
        20,
        remaining_mines_icon_texture.get_y() + remaining_mines_icon_texture.get_h() + 10
    );
}

void init_mouse_controls_textures(SDL_Renderer *renderer, const int window_height) {
    const int icon_height = get_font(Font::PRIMARY).size * 2;

    mouse_left_icon_texture = {renderer, mouse_left_icon_path};
    mouse_left_icon_texture.set_position(20, window_height - icon_height * 2 - 30);
    mouse_left_icon_texture.set_height(icon_height);

    mouse_left_text_texture = {renderer, Font::PRIMARY, "uncover", Color::WHITE};
    mouse_left_text_texture.set_position(
        mouse_left_icon_texture.get_x() + mouse_left_icon_texture.get_w() + 10,
        mouse_left_icon_texture.get_y() + (icon_height - mouse_left_text_texture.get_h()) / 2
    );

    mouse_right_icon_texture = {renderer, mouse_right_icon_path};
    mouse_right_icon_texture.set_position(20, window_height - icon_height - 20);
    mouse_right_icon_texture.set_height(icon_height);

    mouse_right_text_texture = {renderer, Font::PRIMARY, "flag", Color::WHITE};
    mouse_right_text_texture.set_position(
        mouse_right_icon_texture.get_x() + mouse_right_icon_texture.get_w() + 10,
        mouse_right_icon_texture.get_y() + (icon_height - mouse_right_text_texture.get_h()) / 2
    );
}

void init_game_textures(SDL_Renderer *renderer, const Game::Measurements &measurements, const int window_height) {
    init_grid_lines_textures(renderer, measurements);
    init_cell_map_texture(renderer);
    init_cell_numbers_textures(renderer, measurements);
    init_back_button_texture(renderer);
    init_remaining_mines_textures(renderer);
    init_game_time_texture(renderer);
    init_mouse_controls_textures(renderer, window_height);

    init_cell_textures_set(
        renderer,
        measurements,
        cell_textures[GameTexture::CELL_COVERED],
        nullptr,
        0,
        Color::BACKGROUND,
        Color::THEME
    );
    init_cell_textures_set(
        renderer,
        measurements,
        cell_textures[GameTexture::CELL_FLAG],
        flag_image_path,
        0.35,
        Color::FLAGGED_CELL,
        Color::FLAGGED_CELL_BG
    );
    init_cell_textures_set(
        renderer,
        measurements,
        cell_textures[GameTexture::CELL_FLAGGED_MINE],
        mine_image_path,
        0.5,
        Color::FLAGGED_CELL,
        Color::FLAGGED_CELL_BG
    );
    init_cell_textures_set(
        renderer,
        measurements,
        cell_textures[GameTexture::CELL_COVERED_MINE],
        mine_image_path,
        0.5,
        Color::BACKGROUND,
        Color::THEME
    );
    init_cell_textures_set(
        renderer,
        measurements,
        cell_textures[GameTexture::CELL_TRIGGERED_MINE],
        mine_image_path,
        0.5,
        Color::TRIGGERED_MINE,
        Color::TRIGGERED_MINE_BG
    );

    cell_map_texture.destroy();
}

Texture get_cell_texture(const GameTexture::CellSubtype subtype, const GameTexture::CellType type) {
    return cell_textures[subtype][type];
}

Texture get_cell_number_texture(const int number) {
    if (number < 0 || number > 7) {
        std::cerr << "Number must be between 0 and 7 inclusive." << std::endl;
        exit(1);
    }

    return cell_numbers_textures[number];
}

// ReSharper disable once CppNotAllPathsReturnValue
Texture &get_game_texture(const GameTexture::Name name) {
    switch (name) {
        case GameTexture::H_GRID_LINE: return h_grid_line_texture;
        case GameTexture::V_GRID_LINE: return v_grid_line_texture;
        case GameTexture::GAME_TIME_TEXT: return game_time_text_texture;
        case GameTexture::REMAINING_MINES_TEXT: return remaining_mines_text_texture;
        case GameTexture::REMAINING_MINES_ICON: return remaining_mines_icon_texture;
        case GameTexture::MOUSE_LEFT_ICON: return mouse_left_icon_texture;
        case GameTexture::MOUSE_LEFT_TEXT: return mouse_left_text_texture;
        case GameTexture::MOUSE_RIGHT_ICON: return mouse_right_icon_texture;
        case GameTexture::MOUSE_RIGHT_TEXT: return mouse_right_text_texture;
        case GameTexture::BACK_BUTTON: return back_button_texture;
    }
}

void free_game_textures() {
    for (auto &texture : cell_numbers_textures)
        texture.destroy();

    for (auto &texture_set : cell_textures)
        for (auto &texture : texture_set)
            texture.destroy();

    game_time_text_texture.destroy();
    remaining_mines_icon_texture.destroy();
    remaining_mines_text_texture.destroy();
    mouse_left_icon_texture.destroy();
    mouse_left_text_texture.destroy();
    mouse_right_icon_texture.destroy();
    mouse_right_text_texture.destroy();
}

void update_text_texture(
    SDL_Renderer *renderer,
    Texture &texture,
    const Font::Name font,
    const Color::Name color,
    const char *text
) {
    texture.destroy();
    texture = {renderer, font, text, color};
}
