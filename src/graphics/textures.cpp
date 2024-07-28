#include "textures.hpp"

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "fonts.hpp"
#include "../core/game.hpp"

Texture grid_texture;

Texture cell_map_texture;
constexpr int cell_texture_size = 512;

Texture cell_textures[Texture::CELL_SUBTYPES][Texture::CELL_TYPES];

Texture cell_numbers_textures[8];

Texture game_time_text_texture;
Texture remaining_mines_text_texture;
Texture remaining_mines_icon_texture;

const auto cell_map_path = "assets/images/cell_map.png";
const auto mine_image_path = "assets/images/mine.png";
const auto flag_image_path = "assets/images/flag.png";

const auto grid_line_horizontal_image_path = "assets/images/grid_line_horizontal.png";
const auto grid_line_vertical_image_path = "assets/images/grid_line_vertical.png";

const auto grid_filler_horizontal_image_path = "assets/images/grid_filler_horizontal.png";
const auto grid_filler_vertical_image_path = "assets/images/grid_filler_vertical.png";

Color colors[Color::NAMES_AMOUNT];

void init_colors(SDL_Window *window) {
    const SDL_Surface *surface = SDL_GetWindowSurface(window);
    colors[Color::THEME] = {surface, "#d77f37"};
    colors[Color::BACKGROUND] = {surface, "#333333"};

    colors[Color::GRID_1] = {surface, "#b3b3ff"};  // #0000ff
    colors[Color::GRID_2] = {surface, "#b3ffb3"};  // #008000
    colors[Color::GRID_3] = {surface, "#ffb3b3"};  // #ff0000
    colors[Color::GRID_4] = {surface, "#4d4dff"};  // #000080
    colors[Color::GRID_5] = {surface, "#ff4d4d"};  // #800000
    colors[Color::GRID_6] = {surface, "#b3ffff"};  // #008080
    colors[Color::GRID_7] = {surface, "#bfbfbf"};  // #808080
    colors[Color::GRID_8] = {surface, "#ffffff"};

    colors[Color::FLAGGED_CELL] = {surface, "#333333"};
    colors[Color::FLAGGED_CELL_BG] = {surface, "#606060"};
    colors[Color::TRIGGERED_MINE] = {surface, "#431a0d"};
    colors[Color::TRIGGERED_MINE_BG] = {surface, "#b6350d"};

    colors[Color::BLACK] = {surface, "#000000"};
    colors[Color::DARK_GREY] = {surface, "#1e1f1c"};
    colors[Color::GREY] = {surface, "#333333"};
    colors[Color::LIGHT_GREY] = {surface, "#606060"};
    colors[Color::LIGHTER_GREY] = {surface, "#cfcfcf"};
    colors[Color::WHITE] = {surface, "#ffffff"};
}

Color get_color(const Color::Name name) {
    return colors[name];
}

SDL_Texture *create_texture(SDL_Renderer *renderer, const int width, const int height, const int access) {
    const Uint32 pixel_format = SDL_GetWindowSurface(SDL_RenderGetWindow(renderer))->format->format;
    SDL_Texture *texture = SDL_CreateTexture(renderer, pixel_format, access, width, height);
    return texture;
}

// SDL_Surface *create_surface(SDL_Window *window, const int width, const int height) {
//     const Uint32 pixel_format = SDL_GetWindowSurface(window)->format->format;
//     SDL_Surface *m_surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, pixel_format);
//     return m_surface;
// }

// SDL_Surface *create_colored_surface(SDL_Window *window, const int width, const int height, const ColorName color) {
//     SDL_Surface *m_surface = create_surface(window, width, height);
//     const SDL_Rect m_area = {0, 0, width, height};
//     const Uint32 surface_color = get_color(color).value;
//     SDL_FillRect(m_surface, &m_area, surface_color);
//     return m_surface;
// }

// void initCellSizedTextureFromImage(const char *imagePath, Texture *destTexture, const ColorName color) {
//     const int cell_size = game.measurements.cell_size;
//     const int textureSize = cell_size * 0.5;
//     const int grid_line_width = game.measurements.grid_line_width;
//     const int textureOffset = (grid_line_width + cell_size - textureSize) / 2;
//     const SDL_Color textureColor = colors[color].rgb;

//     SDL_Surface *surface = IMG_Load(imagePath);
//     SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
//     SDL_Rect area = SDL_Rect{textureOffset, textureOffset, textureSize, textureSize};

//     SDL_SetTextureColorMod(texture, textureColor.r, textureColor.g, textureColor.b);

//     destTexture->m_area = area;
//     destTexture->m_surface = surface;
//     destTexture->m_texture = texture;
// }

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
    Texture textures[Texture::CELL_TYPES],
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

    Texture image_texture{};

    if (image_path != nullptr && image_scale_wrt_cell != 0) {
        image_texture = {
            renderer,
            image_path,
            {image_offset, image_offset, image_size, image_size},
        };

        image_texture.set_color_mod(get_color(image_color).rgb);
    }

    for (int type = 0; type < Texture::CELL_TYPES; type++) {
        textures[type] = {
            create_texture(renderer, cell_size, cell_size, SDL_TEXTUREACCESS_TARGET),
            texture_area,
        };

        textures[type].set_as_render_target(renderer);

        const int map_index = type * cell_texture_size;
        const int map_x = map_index % cell_map_texture.get_w();
        const int map_y = map_index / cell_map_texture.get_h() * cell_texture_size;

        cell_map_texture.set_color_mod(cell_texture_color);
        cell_map_texture.render(renderer, {map_x, map_y, cell_texture_size, cell_texture_size});

        cell_map_texture.set_color_mod({255, 255, 255});

        if (image_path != nullptr && image_scale_wrt_cell != 0)
            image_texture.render(renderer, NULL_RECT, image_texture.get_area());

        SDL_SetRenderTarget(renderer, nullptr);
    }

    image_texture.destroy();
}

void init_cell_numbers_textures(SDL_Renderer *renderer, const Game::Measurements &measurements) {
    const int cell_size = measurements.cell_size;
    const int grid_line_width = measurements.grid_line_width;

    TTF_Font *cell_sized_font = get_font(Font::RUBIK_MEDIUM_CELL_SIZED).font;

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
        create_texture(renderer, grid_width, grid_height, SDL_TEXTUREACCESS_TARGET),
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
    remaining_mines_icon_texture.set_size(get_font(Font::RUBIK_MEDIUM_PRIMARY).size);
}

void init_textures(SDL_Renderer *renderer, const Game::Measurements &measurements) {
    init_grid_texture(renderer, measurements);
    init_cell_map_texture(renderer);
    init_cell_numbers_textures(renderer, measurements);
    init_remaining_mines_icon_texture(renderer);

    init_cell_textures_set(
        renderer,
        measurements,
        cell_textures[Texture::CELL_COVERED],
        nullptr,
        0,
        Color::BACKGROUND,
        Color::THEME
    );
    init_cell_textures_set(
        renderer,
        measurements,
        cell_textures[Texture::CELL_FLAG],
        flag_image_path,
        0.35,
        Color::FLAGGED_CELL,
        Color::FLAGGED_CELL_BG
    );
    init_cell_textures_set(
        renderer,
        measurements,
        cell_textures[Texture::CELL_FLAGGED_MINE],
        mine_image_path,
        0.5,
        Color::FLAGGED_CELL,
        Color::FLAGGED_CELL_BG
    );
    init_cell_textures_set(
        renderer,
        measurements,
        cell_textures[Texture::CELL_COVERED_MINE],
        mine_image_path,
        0.5,
        Color::BACKGROUND,
        Color::THEME
    );
    init_cell_textures_set(
        renderer,
        measurements,
        cell_textures[Texture::CELL_TRIGGERED_MINE],
        mine_image_path,
        0.5,
        Color::TRIGGERED_MINE,
        Color::TRIGGERED_MINE_BG
    );

    cell_map_texture.destroy();
}

Texture get_cell_texture(const Texture::CellSubtype subtype, const Texture::CellType type) {
    return cell_textures[subtype][type];
}

Texture get_cell_number_texture(const int number) {
    if (number < 0 || number > 7) {
        std::cerr << "Number must be between 0 and 7 inclusive." << std::endl;
        exit(1);
    }

    return cell_numbers_textures[number];
}

Texture &get_texture(const Texture::Name name) {
    switch (name) {
        case Texture::GRID: return grid_texture;
        case Texture::GAME_TIME_TEXT: return game_time_text_texture;
        case Texture::REMAINING_MINES_TEXT: return remaining_mines_text_texture;
        case Texture::REMAINING_MINES_ICON: return remaining_mines_icon_texture;
        default: {
            std::cerr << "Invalid m_texture name " << name << std::endl;
            exit(1);
        }
    }
}

void free_cell_textures_from(Texture textures[Texture::CELL_TYPES]) {
    for (int type = 0; type < Texture::CELL_TYPES; type++)
        textures[type].destroy();
}

void free_cell_numbers_textures() {
    for (int cell = Game::CELL_1; cell <= Game::CELL_8; cell++)
        cell_numbers_textures[cell - Game::CELL_1].destroy();
}

void free_textures() {
    free_cell_numbers_textures();

    free_cell_textures_from(cell_textures[Texture::CELL_COVERED]);
    free_cell_textures_from(cell_textures[Texture::CELL_COVERED_MINE]);
    free_cell_textures_from(cell_textures[Texture::CELL_FLAGGED_MINE]);
    free_cell_textures_from(cell_textures[Texture::CELL_TRIGGERED_MINE]);
    free_cell_textures_from(cell_textures[Texture::CELL_FLAG]);

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
