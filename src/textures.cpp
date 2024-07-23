#include "textures.hpp"

#include <cstdio>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "fonts.hpp"
#include "grid.hpp"
#include "util.hpp"

Texture grid_texture;

Texture cell_map_texture;
constexpr int cell_texture_size = 512;

Texture cell_covered_textures[TEXTURE_CELL_TYPES];
Texture cell_covered_mine_textures[TEXTURE_CELL_TYPES];
Texture cell_flagged_mine_textures[TEXTURE_CELL_TYPES];
Texture cell_triggered_mine_textures[TEXTURE_CELL_TYPES];
Texture cell_flag_textures[TEXTURE_CELL_TYPES];

Texture cell_numbers_textures[8];

Texture game_time_text_texture;
Texture remaining_mines_text_texture;
Texture remaining_mines_icon_texture;

bool textures_ready = false;

auto cell_map_path = "assets/images/cell_map.png";
auto mine_image_path = "assets/images/mine.png";
auto flag_image_path = "assets/images/flag.png";

auto grid_line_horizontal_image_path = "assets/images/grid_line_horizontal.png";
auto grid_line_vertical_image_path = "assets/images/grid_line_vertical.png";

auto grid_filler_horizontal_image_path = "assets/images/grid_filler_horizontal.png";
auto grid_filler_vertical_image_path = "assets/images/grid_filler_vertical.png";

const TextureCellType texture_cell_side_type_order[16] = {
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

const TextureCellType texture_cell_corner_type_order[33] = {
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

void init_texture_from_image(SDL_Renderer *renderer, const char *image_path, Texture *dest_texture) {
    SDL_Surface *surface = IMG_Load(image_path);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    dest_texture->area = {0, 0, surface->w, surface->h};
    dest_texture->surface = surface;
    dest_texture->texture = texture;
}

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

void init_cell_textures_for(
    SDL_Renderer *renderer,
    Texture textures[TEXTURE_CELL_TYPES],
    const char *image_path,
    const float image_scale_wrt_cell,
    const ColorName image_color,
    const ColorName cell_color
) {
    const int cell_size = game.measurements.cell_size;
    const int cell_offset = game.measurements.cell_offset;
    const int grid_line_width = game.measurements.grid_line_width;
    const SDL_Color cell_texture_color = get_color(cell_color).rgb;

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

        if (image_path != nullptr && image_scale_wrt_cell != 0) {
            const int image_size = cell_size * image_scale_wrt_cell;
            const int image_offset = (grid_line_width + cell_size - image_size) / 2 - cell_offset;
            const SDL_Color image_color_rgb = get_color(image_color).rgb;

            SDL_Surface *image_surface = IMG_Load(image_path);
            SDL_Texture *image_texture = SDL_CreateTextureFromSurface(renderer, image_surface);
            SDL_Rect image_area = {image_offset, image_offset, image_size, image_size};

            SDL_SetTextureColorMod(image_texture, image_color_rgb.r, image_color_rgb.g, image_color_rgb.b);
            SDL_RenderCopy(renderer, image_texture, nullptr, &image_area);
        }

        const SDL_Rect texture_area = {cell_offset, cell_offset, cell_size, cell_size};
        SDL_SetRenderTarget(renderer, nullptr);

        textures[type].surface = nullptr;
        textures[type].texture = final_texture;
        textures[type].area = texture_area;
    }
}

void init_cell_numbers_textures(SDL_Renderer *renderer) {
    const int cell_size = game.measurements.cell_size;
    const int grid_line_width = game.measurements.grid_line_width;

    TTF_Font *cell_sized_font = get_font(FONT_RUBIK_MEDIUM_CELL_SIZED).font;

    for (int cell = CELL_1; cell <= CELL_8; cell++) {
        char cell_text[2];
        snprintf(cell_text, 2, "%c", '0' + cell - CELL_0);
        const auto [rgb, value] = get_color(static_cast<ColorName>(COLOR_GRID_1 + cell - 1));

        SDL_Surface *text_surface = TTF_RenderText_Solid(cell_sized_font, cell_text, rgb);
        SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

        SDL_Rect cell_area;
        TTF_SizeText(cell_sized_font, cell_text, &cell_area.w, &cell_area.h);

        cell_area.x = (grid_line_width + cell_size - cell_area.w) / 2;
        cell_area.y = (grid_line_width + cell_size - cell_area.h) / 2;

        cell_numbers_textures[cell - CELL_1] = {
            .surface = text_surface,
            .texture = text_texture,
            .area = cell_area,
        };
    }
}

void init_grid_texture(SDL_Renderer *renderer) {
    const int cell_size = game.measurements.cell_size;
    const int grid_line_length = game.measurements.grid_line_length;
    const int grid_line_width = game.measurements.grid_line_width;
    const int grid_line_offset = (grid_line_width + cell_size - grid_line_length) / 2;
    const int grid_x_offset = game.measurements.grid_x_offset;
    const int grid_y_offset = game.measurements.grid_y_offset;
    const int grid_width = game.measurements.grid_width;
    const int grid_height = game.measurements.grid_height;
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

void init_textures(SDL_Renderer *renderer) {
    if (textures_ready)
        return;

    init_grid_texture(renderer);

    init_cell_map_texture(renderer);

    init_cell_textures_for(renderer, cell_covered_textures, nullptr, 0, COLOR_BACKGROUND, COLOR_THEME);
    init_cell_textures_for(
        renderer,
        cell_flag_textures,
        flag_image_path,
        0.35,
        COLOR_FLAGGED_CELL,
        COLOR_FLAGGED_CELL_BG
    );
    init_cell_textures_for(
        renderer,
        cell_flagged_mine_textures,
        mine_image_path,
        0.5,
        COLOR_FLAGGED_CELL,
        COLOR_FLAGGED_CELL_BG
    );
    init_cell_textures_for(renderer, cell_covered_mine_textures, mine_image_path, 0.5, COLOR_BACKGROUND, COLOR_THEME);
    init_cell_textures_for(
        renderer,
        cell_triggered_mine_textures,
        mine_image_path,
        0.5,
        COLOR_TRIGGERED_MINE,
        COLOR_TRIGGERED_MINE_BG
    );

    init_cell_numbers_textures(renderer);

    init_remaining_mines_icon_texture(renderer);

    textures_ready = true;
}

void free_texture(const Texture &texture) {
    SDL_FreeSurface(texture.surface);
    SDL_DestroyTexture(texture.texture);
}

void free_cell_textures_from(Texture textures[TEXTURE_CELL_TYPES]) {
    for (int type = 0; type < TEXTURE_CELL_TYPES; type++)
        free_texture(textures[type]);
}

void free_cell_numbers_textures() {
    for (int cell = CELL_1; cell <= CELL_8; cell++) {
        const Texture cell_texture = cell_numbers_textures[cell - CELL_1];
        free_texture(cell_texture);
    }
}

void free_textures() {
    free_cell_numbers_textures();

    free_cell_textures_from(cell_covered_textures);
    free_cell_textures_from(cell_covered_mine_textures);
    free_cell_textures_from(cell_flagged_mine_textures);
    free_cell_textures_from(cell_triggered_mine_textures);
    free_cell_textures_from(cell_flag_textures);

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
