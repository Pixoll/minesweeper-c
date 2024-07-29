#include "game_textures.hpp"

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "colors.hpp"
#include "fonts.hpp"
#include "texture.hpp"
#include "../core/game.hpp"

Texture grid_texture;

Texture cell_map_texture;
constexpr int cell_texture_size = 512;

Texture cell_textures[GameTexture::CELL_SUBTYPES][GameTexture::CELL_TYPES];

Texture cell_numbers_textures[8];

Texture game_time_text_texture;
Texture remaining_mines_text_texture;
Texture remaining_mines_icon_texture;

const auto cell_map_path = "assets/textures/cell_map.png";
const auto mine_image_path = "assets/textures/mine.png";
const auto flag_image_path = "assets/textures/flag.png";

const auto grid_line_horizontal_image_path = "assets/textures/grid_line_horizontal.png";
const auto grid_line_vertical_image_path = "assets/textures/grid_line_vertical.png";

void init_cell_map_texture(SDL_Renderer *renderer) {
    SDL_Surface *surface = IMG_Load(cell_map_path);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    cell_map_texture = {
        surface,
        texture,
        {0, 0, surface->w, surface->h},
    };
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
    const SDL_Color cell_texture_color = get_color(cell_color).rgb;
    const SDL_Color white = get_color(Color::WHITE).rgb;

    Texture image_texture{};

    if (image_path != nullptr && image_scale_wrt_cell != 0) {
        image_texture = {
            renderer,
            image_path,
            {image_offset, image_offset, image_size, image_size},
        };

        image_texture.set_color_mod(get_color(image_color).rgb);
    }

    for (int type = 0; type < GameTexture::CELL_TYPES; type++) {
        textures[type] = {renderer, texture_area};

        textures[type].set_as_render_target(renderer);

        const int map_index = type * cell_texture_size;
        const int map_x = map_index % cell_map_texture.get_w();
        const int map_y = map_index / cell_map_texture.get_h() * cell_texture_size;

        cell_map_texture.set_color_mod(cell_texture_color);
        cell_map_texture.render(renderer, {map_x, map_y, cell_texture_size, cell_texture_size});

        cell_map_texture.set_color_mod(white);

        if (image_path != nullptr && image_scale_wrt_cell != 0)
            image_texture.render(renderer);

        SDL_SetRenderTarget(renderer, nullptr);
    }

    image_texture.destroy();
}

void init_cell_numbers_textures(SDL_Renderer *renderer, const Game::Measurements &measurements) {
    const int cell_size = measurements.cell_size;
    const int grid_line_width = measurements.grid_line_width;

    TTF_Font *cell_sized_font = get_font(Font::CELL_NUMBER).font;

    for (int cell = Game::CELL_1; cell <= Game::CELL_8; cell++) {
        char cell_text[2];
        snprintf(cell_text, 2, "%c", '0' + cell - Game::CELL_0);
        const auto [rgb, value] = get_color(static_cast<Color::Name>(Color::GRID_1 + cell - Game::CELL_1));

        SDL_Surface *text_surface = TTF_RenderText_Solid(cell_sized_font, cell_text, rgb);
        SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

        SDL_Rect cell_area;
        TTF_SizeText(cell_sized_font, cell_text, &cell_area.w, &cell_area.h);

        cell_area.x = (grid_line_width + cell_size - cell_area.w) / 2;
        cell_area.y = (grid_line_width + cell_size - cell_area.h) / 2;

        cell_numbers_textures[cell - Game::CELL_1] = {text_surface, text_texture, cell_area};
    }
}

void init_grid_texture(SDL_Renderer *renderer, const Game::Measurements &measurements) {
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

    grid_texture = {
        renderer,
        {grid_x_offset, grid_y_offset, grid_width, grid_height},
    };

    grid_texture.set_as_render_target(renderer);

    Texture grid_line_h_texture(renderer, grid_line_horizontal_image_path);
    grid_line_h_texture.set_color_mod(light_grey);

    Texture grid_line_v_texture(renderer, grid_line_vertical_image_path);
    grid_line_v_texture.set_color_mod(light_grey);

    for (int x = 0; x < grid_width - grid_line_width; x += cell_size) {
        for (int y = 0; y < grid_height - grid_line_width; y += cell_size) {
            if (x > 0)
                grid_line_v_texture.render(
                    renderer,
                    NULL_RECT,
                    {x, y + grid_line_offset, grid_line_width, grid_line_length}
                );

            if (y > 0)
                grid_line_h_texture.render(
                    renderer,
                    NULL_RECT,
                    {x + grid_line_offset, y, grid_line_length, grid_line_width}
                );
        }
    }

    SDL_SetRenderTarget(renderer, nullptr);
    grid_line_h_texture.destroy();
    grid_line_v_texture.destroy();
}

void init_remaining_mines_icon_texture(SDL_Renderer *renderer) {
    remaining_mines_icon_texture = {renderer, mine_image_path};
    remaining_mines_icon_texture.set_size(get_font(Font::PRIMARY).size);
}

void init_game_textures(SDL_Renderer *renderer, const Game::Measurements &measurements) {
    init_grid_texture(renderer, measurements);
    init_cell_map_texture(renderer);
    init_cell_numbers_textures(renderer, measurements);
    init_remaining_mines_icon_texture(renderer);

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

Texture &get_game_texture(const GameTexture::Name name) {
    switch (name) {
        case GameTexture::GRID: return grid_texture;
        case GameTexture::GAME_TIME_TEXT: return game_time_text_texture;
        case GameTexture::REMAINING_MINES_TEXT: return remaining_mines_text_texture;
        case GameTexture::REMAINING_MINES_ICON: return remaining_mines_icon_texture;
        default: {
            std::cerr << "Invalid m_texture name " << name << std::endl;
            exit(1);
        }
    }
}

void free_cell_textures_from(Texture textures[GameTexture::CELL_TYPES]) {
    for (int type = 0; type < GameTexture::CELL_TYPES; type++)
        textures[type].destroy();
}

void free_cell_numbers_textures() {
    for (int cell = Game::CELL_1; cell <= Game::CELL_8; cell++)
        cell_numbers_textures[cell - Game::CELL_1].destroy();
}

void free_game_textures() {
    free_cell_numbers_textures();

    free_cell_textures_from(cell_textures[GameTexture::CELL_COVERED]);
    free_cell_textures_from(cell_textures[GameTexture::CELL_COVERED_MINE]);
    free_cell_textures_from(cell_textures[GameTexture::CELL_FLAGGED_MINE]);
    free_cell_textures_from(cell_textures[GameTexture::CELL_TRIGGERED_MINE]);
    free_cell_textures_from(cell_textures[GameTexture::CELL_FLAG]);

    grid_texture.destroy();
    game_time_text_texture.destroy();
    remaining_mines_text_texture.destroy();
}

void update_text_texture(
    SDL_Renderer *renderer,
    Texture &texture,
    const Font::Name font_name,
    const Color::Name color,
    const char *text
) {
    texture.destroy();

    SDL_Surface *text_surface = TTF_RenderText_Solid(get_font(font_name).font, text, get_color(color).rgb);
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

    texture = {
        text_surface,
        text_texture,
        {0, 0, text_surface->w, text_surface->h},
    };
}
