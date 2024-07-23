#include <cstdlib>
#include <iostream>
#include <string>

#include "global.hpp"
#include "grid.hpp"
#include "textures.hpp"
#include "util.hpp"

using std::cerr, std::endl, std::string;

time_t last_game_time_drawn = 0;
int remaining_mines = 0;

void draw_grid();
void draw_remaining_mines();
void draw_game_time();

void draw_grid_ui() {
    draw_grid();
    draw_remaining_mines();
    draw_game_time();
}

Texture get_cell_texture(GridCell cell, TEXTURE_CELL_TYPE type);
TEXTURE_CELL_TYPE get_cell_type(int x, int y, bool flagged, bool revealed);

void draw_grid() {
    const int rows = game.rows;
    const int columns = game.columns;
    const int cell_size = game.measurements.cell_size;
    const int grid_x_offset = game.measurements.grid_x_offset;
    const int grid_y_offset = game.measurements.grid_y_offset;

    // Draw grid
    SDL_RenderCopy(renderer, grid_texture.texture, nullptr, &grid_texture.area);

    // Draw cells
    for (int i = 0; i < columns; i++) {
        const int x = grid_x_offset + cell_size * i;

        for (int j = 0; j < rows; j++) {
            const int y = grid_y_offset + cell_size * j;
            const GridCell cell = game.grid[i][j];

            if (cell.type == CELL_0 && cell.revealed)
                continue;

            const TEXTURE_CELL_TYPE cell_type = get_cell_type(i, j, cell.flagged, cell.revealed);
            Texture cell_texture = get_cell_texture(cell, cell_type);
            cell_texture.area.x += x;
            cell_texture.area.y += y;

            SDL_RenderCopy(renderer, cell_texture.texture, nullptr, &cell_texture.area);
        }
    }
}

void draw_remaining_mines() {
    const int current_remaining = game.total_mines - game.flagged_mines;

    if (remaining_mines != current_remaining) {
        remaining_mines = current_remaining;
        char remaining_string[int_length(current_remaining) + 1];
        itoa(current_remaining, remaining_string, 10);
        update_text_texture(&remaining_mines_text_texture, font_rubik_medium_primary, COLOR_WHITE, remaining_string);
    }

    // TODO Icon and text aren't centered with each other, I hate it
    remaining_mines_icon_texture.area.x = 10;
    remaining_mines_icon_texture.area.y = 10;
    SDL_RenderCopy(renderer, remaining_mines_icon_texture.texture, nullptr, &remaining_mines_icon_texture.area);

    remaining_mines_text_texture.area.x = remaining_mines_icon_texture.area.w + 20;
    remaining_mines_text_texture.area.y = 10;
    SDL_RenderCopy(renderer, remaining_mines_text_texture.texture, nullptr, &remaining_mines_text_texture.area);
}

void draw_game_time() {
    if (game.start_time == 0)
        return;

    const time_t now = time(nullptr);
    if (last_game_time_drawn == 0 || (!game.over && last_game_time_drawn < now)) {
        last_game_time_drawn = now;
        const string time_string = get_time_string(now - game.start_time);
        update_text_texture(
            &game_time_text_texture,
            font_rubik_medium_secondary,
            COLOR_LIGHTER_GREY,
            time_string.c_str()
        );
    }

    game_time_text_texture.area.x = 10;
    game_time_text_texture.area.y = game_time_text_texture.area.h + 20;
    SDL_RenderCopy(renderer, game_time_text_texture.texture, nullptr, &game_time_text_texture.area);
}

Texture get_cell_texture(const GridCell cell, const TEXTURE_CELL_TYPE type) {
    if (game.over && !game.won && cell.type == CELL_MINE) {
        if (cell.flagged)
            return cell_flagged_mine_textures[type];

        if (cell.revealed)
            return cell_triggered_mine_textures[type];

        return cell_covered_mine_textures[type];
    }

    if (cell.flagged)
        return cell_flag_textures[type];

    if (!cell.revealed)
        return cell_covered_textures[type];

    return cell_numbers_textures[cell.type - CELL_1];
}

bool verify_cell(const int x, const int y, const bool flagged) {
    return !game.grid[x][y].revealed && flagged == game.grid[x][y].flagged;
}

bool verify_corners_with_mask(const int corners, const int mask) {
    const bool bits_outside_of_mask = corners & ~mask;
    const bool bits_in_mask = corners & mask;
    return !bits_outside_of_mask && bits_in_mask;
}

TEXTURE_CELL_TYPE get_cell_type(const int x, const int y, const bool flagged, const bool revealed) {
    if (revealed)
        return TEXTURE_CELL_NO_SIDES;

    const int rows = game.rows;
    const int columns = game.columns;

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
        return texture_cell_side_type_order[TBLR];
    if (TBLR == 0b1111)
        return texture_cell_corner_type_order[TLR_BLR_C - 1];

    if (TBLR == 0b0111 && verify_corners_with_mask(TLR_BLR_C, 0b0011))
        return texture_cell_corner_type_order[TLR_BLR_C + 14];
    if (TBLR == 0b1011 && verify_corners_with_mask(TLR_BLR_C, 0b1100))
        return texture_cell_corner_type_order[TLR_BLR_C + 15];
    if (TBLR == 0b1101 && verify_corners_with_mask(TLR_BLR_C, 0b0101))
        return texture_cell_corner_type_order[TLR_BLR_C + 17];
    if (TBLR == 0b1110 && verify_corners_with_mask(TLR_BLR_C, 0b1010))
        return texture_cell_corner_type_order[TLR_BLR_C + 18];

    if (is_pow2(TLR_BLR_C))
        return texture_cell_corner_type_order[int_log2(TLR_BLR_C) + 29];

    // Impossible to reach? Not reached in huge grid
    cerr << """Not impossible to reach" << endl;

    return TEXTURE_CELL_NO_SIDES;
}
