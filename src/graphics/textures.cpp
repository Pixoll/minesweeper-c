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

Texture cell_textures[TEXTURE_CELL_SUBTYPES][TEXTURE_CELL_TYPES];

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

Color::Color(const SDL_Surface *surface, const char *hex_color) {
    if (hex_color[0] == '#')
        hex_color++;  // shift left once

    const int rgb = strtol(hex_color, nullptr, 16);
    const Uint8 r = rgb >> 16 & 0xff;
    const Uint8 g = rgb >> 8 & 0xff;
    const Uint8 b = rgb & 0xff;

    this->rgb = {r, g, b, 255};
    this->value = SDL_MapRGB(surface->format, r, g, b);
}

Color colors[COLORS_AMOUNT];

void init_colors(SDL_Window *window) {
    const SDL_Surface *surface = SDL_GetWindowSurface(window);
    colors[COLOR_THEME] = Color(surface, "#d77f37");
    colors[COLOR_BACKGROUND] = Color(surface, "#333333");

    colors[COLOR_GRID_1] = Color(surface, "#b3b3ff");  // #0000ff
    colors[COLOR_GRID_2] = Color(surface, "#b3ffb3");  // #008000
    colors[COLOR_GRID_3] = Color(surface, "#ffb3b3");  // #ff0000
    colors[COLOR_GRID_4] = Color(surface, "#4d4dff");  // #000080
    colors[COLOR_GRID_5] = Color(surface, "#ff4d4d");  // #800000
    colors[COLOR_GRID_6] = Color(surface, "#b3ffff");  // #008080
    colors[COLOR_GRID_7] = Color(surface, "#bfbfbf");  // #808080
    colors[COLOR_GRID_8] = Color(surface, "#ffffff");

    colors[COLOR_FLAGGED_CELL] = Color(surface, "#333333");
    colors[COLOR_FLAGGED_CELL_BG] = Color(surface, "#606060");
    colors[COLOR_TRIGGERED_MINE] = Color(surface, "#431a0d");
    colors[COLOR_TRIGGERED_MINE_BG] = Color(surface, "#b6350d");

    colors[COLOR_BLACK] = Color(surface, "#000000");
    colors[COLOR_DARK_GREY] = Color(surface, "#1e1f1c");
    colors[COLOR_GREY] = Color(surface, "#333333");
    colors[COLOR_LIGHT_GREY] = Color(surface, "#606060");
    colors[COLOR_LIGHTER_GREY] = Color(surface, "#cfcfcf");
    colors[COLOR_WHITE] = Color(surface, "#ffffff");
}

Color get_color(const ColorName name) {
    return colors[name];
}

SDL_Texture *create_texture(SDL_Renderer *renderer, const int width, const int height, const int access) {
    const Uint32 pixel_format = SDL_GetWindowSurface(SDL_RenderGetWindow(renderer))->format->format;
    SDL_Texture *texture = SDL_CreateTexture(renderer, pixel_format, access, width, height);
    return texture;
}

void init_texture_from_image(SDL_Renderer *renderer, const char *image_path, Texture *dest_texture) {
    SDL_Surface *surface = IMG_Load(image_path);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    dest_texture->area = {0, 0, surface->w, surface->h};
    dest_texture->surface = surface;
    dest_texture->texture = texture;
}

// SDL_Surface *create_surface(SDL_Window *window, const int width, const int height) {
//     const Uint32 pixel_format = SDL_GetWindowSurface(window)->format->format;
//     SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, pixel_format);
//     return surface;
// }

// SDL_Surface *create_colored_surface(SDL_Window *window, const int width, const int height, const ColorName color) {
//     SDL_Surface *surface = create_surface(window, width, height);
//     const SDL_Rect area = {0, 0, width, height};
//     const Uint32 surface_color = get_color(color).value;
//     SDL_FillRect(surface, &area, surface_color);
//     return surface;
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

//     destTexture->area = area;
//     destTexture->surface = surface;
//     destTexture->texture = texture;
// }

void init_cell_map_texture(SDL_Renderer *renderer) {
    SDL_Surface *surface = IMG_Load(cell_map_path);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    cell_map_texture.area = {0, 0, surface->w, surface->h};
    cell_map_texture.surface = surface;
    cell_map_texture.texture = texture;
}

void free_texture(Texture &texture) {
    if (texture.surface != nullptr)
        SDL_FreeSurface(texture.surface);
    if (texture.texture != nullptr)
        SDL_DestroyTexture(texture.texture);

    texture.surface = nullptr;
    texture.texture = nullptr;
}

void init_cell_textures_set(
    SDL_Renderer *renderer,
    const Game::Measurements &measurements,
    Texture textures[TEXTURE_CELL_TYPES],
    const char *image_path,
    const float image_scale_wrt_cell,
    const ColorName image_color,
    const ColorName cell_color
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
        image_texture.surface = IMG_Load(image_path);
        image_texture.texture = SDL_CreateTextureFromSurface(renderer, image_texture.surface);
        image_texture.area = {image_offset, image_offset, image_size, image_size};

        const auto [r, g, b, a] = get_color(image_color).rgb;
        SDL_SetTextureColorMod(image_texture.texture, r, g, b);
    }

    for (int type = 0; type < TEXTURE_CELL_TYPES; type++) {
        SDL_Texture *final_texture = create_texture(renderer, cell_size, cell_size, SDL_TEXTUREACCESS_TARGET);
        SDL_SetRenderTarget(renderer, final_texture);
        SDL_SetTextureBlendMode(final_texture, SDL_BLENDMODE_BLEND);

        const int map_index = type * cell_texture_size;
        const int map_x = map_index % cell_map_texture.area.w;
        const int map_y = map_index / cell_map_texture.area.h * cell_texture_size;
        const SDL_Rect cell_texture_area = {map_x, map_y, cell_texture_size, cell_texture_size};

        SDL_SetTextureColorMod(
            cell_map_texture.texture,
            cell_texture_color.r,
            cell_texture_color.g,
            cell_texture_color.b
        );
        SDL_RenderCopy(renderer, cell_map_texture.texture, &cell_texture_area, nullptr);
        SDL_SetTextureColorMod(cell_map_texture.texture, 255, 255, 255);

        if (image_path != nullptr && image_scale_wrt_cell != 0)
            SDL_RenderCopy(renderer, image_texture.texture, nullptr, &image_texture.area);

        SDL_SetRenderTarget(renderer, nullptr);

        textures[type].surface = nullptr;
        textures[type].texture = final_texture;
        textures[type].area = texture_area;
    }

    free_texture(image_texture);
}

void init_cell_numbers_textures(SDL_Renderer *renderer, const Game::Measurements &measurements) {
    const int cell_size = measurements.cell_size;
    const int grid_line_width = measurements.grid_line_width;

    TTF_Font *cell_sized_font = get_font(FONT_RUBIK_MEDIUM_CELL_SIZED).font;

    for (int cell = Game::CELL_1; cell <= Game::CELL_8; cell++) {
        char cell_text[2];
        snprintf(cell_text, 2, "%c", '0' + cell - Game::CELL_0);
        const auto [rgb, value] = get_color(static_cast<ColorName>(COLOR_GRID_1 + cell - 1));

        SDL_Surface *text_surface = TTF_RenderText_Solid(cell_sized_font, cell_text, rgb);
        SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

        SDL_Rect cell_area;
        TTF_SizeText(cell_sized_font, cell_text, &cell_area.w, &cell_area.h);

        cell_area.x = (grid_line_width + cell_size - cell_area.w) / 2;
        cell_area.y = (grid_line_width + cell_size - cell_area.h) / 2;

        cell_numbers_textures[cell - Game::CELL_1] = {text_surface, text_texture, cell_area,};
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
    const auto [r, g, b, a] = get_color(COLOR_LIGHT_GREY).rgb;

    SDL_Texture *final_texture = create_texture(renderer, grid_width, grid_height, SDL_TEXTUREACCESS_TARGET);
    SDL_SetRenderTarget(renderer, final_texture);
    SDL_SetTextureBlendMode(final_texture, SDL_BLENDMODE_BLEND);

    SDL_Surface *grid_line_h_surface = IMG_Load(grid_line_horizontal_image_path);
    SDL_Texture *grid_line_h_texture = SDL_CreateTextureFromSurface(renderer, grid_line_h_surface);
    SDL_SetTextureColorMod(grid_line_h_texture, r, g, b);

    SDL_Surface *grid_line_v_surface = IMG_Load(grid_line_vertical_image_path);
    SDL_Texture *grid_line_v_texture = SDL_CreateTextureFromSurface(renderer, grid_line_v_surface);
    SDL_SetTextureColorMod(grid_line_v_texture, r, g, b);

    for (int x = 0; x < grid_width - grid_line_width; x += cell_size) {
        for (int y = 0; y < grid_height - grid_line_width; y += cell_size) {
            if (x > 0) {
                const SDL_Rect vertical = {x, y + grid_line_offset, grid_line_width, grid_line_length};
                SDL_RenderCopy(renderer, grid_line_v_texture, nullptr, &vertical);
            }
            if (y > 0) {
                const SDL_Rect horizontal = {x + grid_line_offset, y, grid_line_length, grid_line_width};
                SDL_RenderCopy(renderer, grid_line_h_texture, nullptr, &horizontal);
            }
        }
    }

    SDL_SetRenderTarget(renderer, nullptr);

    const SDL_Rect grid_area = {grid_x_offset, grid_y_offset, grid_width, grid_height};

    grid_texture.surface = nullptr;
    grid_texture.texture = final_texture;
    grid_texture.area = grid_area;
}

void init_remaining_mines_icon_texture(SDL_Renderer *renderer) {
    init_texture_from_image(renderer, mine_image_path, &remaining_mines_icon_texture);
    const int size = get_font(FONT_RUBIK_MEDIUM_PRIMARY).size;
    remaining_mines_icon_texture.area.w = size;
    remaining_mines_icon_texture.area.h = size;
}

void init_textures(SDL_Renderer *renderer, const Game::Measurements &measurements) {
    init_grid_texture(renderer, measurements);
    init_cell_map_texture(renderer);
    init_cell_numbers_textures(renderer, measurements);
    init_remaining_mines_icon_texture(renderer);

    init_cell_textures_set(
        renderer,
        measurements,
        cell_textures[TEXTURE_CELL_COVERED],
        nullptr,
        0,
        COLOR_BACKGROUND,
        COLOR_THEME
    );
    init_cell_textures_set(
        renderer,
        measurements,
        cell_textures[TEXTURE_CELL_FLAG],
        flag_image_path,
        0.35,
        COLOR_FLAGGED_CELL,
        COLOR_FLAGGED_CELL_BG
    );
    init_cell_textures_set(
        renderer,
        measurements,
        cell_textures[TEXTURE_CELL_FLAGGED_MINE],
        mine_image_path,
        0.5,
        COLOR_FLAGGED_CELL,
        COLOR_FLAGGED_CELL_BG
    );
    init_cell_textures_set(
        renderer,
        measurements,
        cell_textures[TEXTURE_CELL_COVERED_MINE],
        mine_image_path,
        0.5,
        COLOR_BACKGROUND,
        COLOR_THEME
    );
    init_cell_textures_set(
        renderer,
        measurements,
        cell_textures[TEXTURE_CELL_TRIGGERED_MINE],
        mine_image_path,
        0.5,
        COLOR_TRIGGERED_MINE,
        COLOR_TRIGGERED_MINE_BG
    );
}

Texture get_cell_texture(const TextureCellSubtype subtype, const TextureCellType type) {
    return cell_textures[subtype][type];
}

Texture get_cell_number_texture(const int number) {
    if (number < 0 || number > 7) {
        std::cerr << "Number must be between 0 and 7 inclusive." << std::endl;
        exit(1);
    }

    return cell_numbers_textures[number];
}

Texture &get_texture(const TextureName name) {
    switch (name) {
        case TEXTURE_GRID: return grid_texture;
        case TEXTURE_GAME_TIME_TEXT: return game_time_text_texture;
        case TEXTURE_REMAINING_MINES_TEXT: return remaining_mines_text_texture;
        case TEXTURE_REMAINING_MINES_ICON: return remaining_mines_icon_texture;
        default: {
            std::cerr << "Invalid texture name " << name << std::endl;
            exit(1);
        }
    }
}

void free_cell_textures_from(Texture textures[TEXTURE_CELL_TYPES]) {
    for (int type = 0; type < TEXTURE_CELL_TYPES; type++)
        free_texture(textures[type]);
}

void free_cell_numbers_textures() {
    for (int cell = Game::CELL_1; cell <= Game::CELL_8; cell++) {
        Texture cell_texture = cell_numbers_textures[cell - Game::CELL_1];
        free_texture(cell_texture);
    }
}

void free_textures() {
    free_cell_numbers_textures();

    free_cell_textures_from(cell_textures[TEXTURE_CELL_COVERED]);
    free_cell_textures_from(cell_textures[TEXTURE_CELL_COVERED_MINE]);
    free_cell_textures_from(cell_textures[TEXTURE_CELL_FLAGGED_MINE]);
    free_cell_textures_from(cell_textures[TEXTURE_CELL_TRIGGERED_MINE]);
    free_cell_textures_from(cell_textures[TEXTURE_CELL_FLAG]);

    free_texture(cell_map_texture);

    free_texture(grid_texture);

    free_texture(game_time_text_texture);
    free_texture(remaining_mines_text_texture);
}

void update_text_texture(
    SDL_Renderer *renderer,
    Texture *texture,
    const FontName font_name,
    const ColorName color,
    const char *text
) {
    if (texture->texture != nullptr)
        free_texture(*texture);

    SDL_Surface *text_surface = TTF_RenderText_Solid(get_font(font_name).font, text, get_color(color).rgb);
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    const SDL_Rect area = {0, 0, text_surface->w, text_surface->h};

    texture->surface = text_surface;
    texture->texture = text_texture;
    texture->area = area;
}
