#include "grid.hpp"

#include <cstdlib>
#include <ctime>
#include <iostream>

Game_t game{};

bool created_grid = false;
bool grid_measurements_ready = false;

const Game_t &get_game() {
    return game;
}

CellType count_surrounding_mines(int x, int y);

void create_grid(const int rows, const int columns, const int mines_count) {
    if (created_grid) {
        for (int i = 0; i < columns; i++)
            game.grid[i].clear();
        game.grid.clear();
    }

    game.grid.reserve(columns);

    for (int i = 0; i < columns; i++)
        game.grid.emplace_back(rows, GridCell{});

    game.rows = rows;
    game.columns = columns;
    game.total_mines = mines_count;
    game.unrevealed_count = rows * columns;
    created_grid = true;
}

int random_between(const int min, const int max) {
    return rand() % (max - min + 1) + min;
}

void place_grid_mines(const int x, const int y) {
    const int rows = game.rows;
    const int columns = game.columns;
    const int count = game.total_mines;

    const time_t now = time(nullptr);
    game.start_time = now;

    srand(now);

    int placed_mines = 0;
    // TODO Is this bad???
    while (placed_mines < count) {
        const int nx = random_between(0, columns - 1);
        const int ny = random_between(0, rows - 1);

        // Mines count at (x, y) must be 0
        if (nx >= x - 1 && nx <= x + 1 && ny >= y - 1 && ny <= y + 1)
            continue;

        if (game.grid[nx][ny].type == CELL_0) {
            game.grid[nx][ny].type = CELL_MINE;
            placed_mines++;
        }
    }

    // Count surrounding mines
    for (int i = 0; i < columns; i++) {
        for (int j = 0; j < rows; j++) {
            const CellType cell = game.grid[i][j].type;
            if (cell == CELL_MINE)
                continue;

            const CellType surrounding = count_surrounding_mines(i, j);
            game.grid[i][j].type = surrounding;
        }
    }
}

void calculate_grid_measurements(const int window_width, const int window_height) {
    if (grid_measurements_ready)
        return;

    const int rows = game.rows;
    const int columns = game.columns;

    const float grid_ratio = static_cast<float>(columns) / rows;
    const float window_ratio = static_cast<float>(window_width) / window_height;

    const int limitant_grid_side = grid_ratio > window_ratio ? columns : rows;
    const int limitant_window_side = grid_ratio > window_ratio ? window_width : window_height;
    const int cell_size = limitant_window_side * 0.975 / limitant_grid_side;

    const int grid_line_length = cell_size * 0.65;
    const int grid_line_width = cell_size * 0.03;
    const int grid_width = cell_size * columns + grid_line_width;
    const int grid_x_offset = (window_width - grid_width) / 2;
    const int grid_height = cell_size * rows + grid_line_width;
    const int grid_y_offset = (window_height - grid_height) / 2;

    const int cell_offset = grid_line_width / 2;

    game.measurements.cell_size = cell_size;
    game.measurements.cell_offset = cell_offset;
    game.measurements.grid_line_length = grid_line_length;
    game.measurements.grid_line_width = grid_line_width;
    game.measurements.grid_x_offset = grid_x_offset;
    game.measurements.grid_y_offset = grid_y_offset;
    game.measurements.grid_width = grid_width;
    game.measurements.grid_height = grid_height;

    grid_measurements_ready = true;
}

GridCoords calculate_grid_cell(const int click_x, const int click_y) {
    const int rows = game.rows;
    const int columns = game.columns;
    const int cell_size = game.measurements.cell_size;
    const int grid_x_offset = game.measurements.grid_x_offset;
    const int grid_y_offset = game.measurements.grid_y_offset;

    bool inside = true;
    float x = static_cast<float>(click_x - grid_x_offset) / cell_size;
    float y = static_cast<float>(click_y - grid_y_offset) / cell_size;
    if (x < 0 || x > columns - 0.01f || y < 0 || y > rows - 0.01f) {
        x = -1;
        y = -1;
        inside = false;
    }

    return {static_cast<int>(x), static_cast<int>(y), inside};
}

void get_surrounding_unrevealed(int x, int y, GridCoords *coords, int *counter);

void toggle_cell_flag(const int x, const int y) {
    const auto [type, flagged, revealed] = game.grid[x][y];
    if (!revealed) {
        game.grid[x][y].flagged = !flagged;

        if (!flagged)
            game.flagged_mines++;
        else
            game.flagged_mines--;

        return;
    }

    if (type < CELL_1 || type > CELL_8)
        return;

    GridCoords unrevealed[9];
    int unrevealed_count = 0;
    get_surrounding_unrevealed(x, y, unrevealed, &unrevealed_count);

    if (unrevealed_count != type - CELL_0)
        return;

    for (int i = 0; i < unrevealed_count; i++) {
        const int nx = unrevealed[i].x;
        const int ny = unrevealed[i].y;

        if (game.grid[nx][ny].flagged)
            continue;

        game.grid[nx][ny].flagged = true;
        game.flagged_mines++;
    }
}

void get_surrounding_unrevealed(const int x, const int y, GridCoords *coords, int *counter) {
    const int rows = game.rows;
    const int columns = game.columns;

    for (int i = -1; i <= 1; i++) {
        const int nx = x + i;

        if (nx < 0 || nx > columns - 1)
            continue;

        for (int j = -1; j <= 1; j++) {
            const int ny = y + j;

            if (ny < 0 || ny > rows - 1 || game.grid[nx][ny].revealed)
                continue;

            coords[*counter] = {nx, ny};
            *counter = *counter + 1;
        }
    }
}

int count_surrounding_flagged(int x, int y);
GridCoords get_surrounding_empty(int x, int y);
bool reveal_non_flagged(int x, int y, GridCoords *coords, int *counter);
void reveal_cells_dfs(int x, int y);
void reveal_cell_border(int x, int y);
void flag_all_unrevealed();

void reveal_cell(const int x, const int y) {
    const auto [type, flagged, revealed] = game.grid[x][y];

    if (flagged)
        return;

    if (type == CELL_MINE) {
        game.grid[x][y].revealed = true;
        game.over = true;
        game.won = false;
        return;
    }

    GridCoords revealed_cells[9] = {{x, y}};
    int revealed_cells_count = 1;

    if (revealed) {
        if (type < CELL_1 || type > CELL_8)
            return;

        const int flagged_count = count_surrounding_flagged(x, y);

        if (flagged_count != type - CELL_0)
            return;

        if (reveal_non_flagged(x, y, revealed_cells, &revealed_cells_count)) {
            game.over = true;
            game.won = false;
            return;
        }
    } else {
        game.grid[x][y].revealed = true;
        game.unrevealed_count--;
    }

    for (int i = 0; i < revealed_cells_count; i++) {
        const int nx = revealed_cells[i].x;
        const int ny = revealed_cells[i].y;

        if (game.grid[nx][ny].type != CELL_0)
            continue;

        reveal_cell_border(nx, ny);
        reveal_cells_dfs(nx, ny);
    }

    if (game.total_mines == game.unrevealed_count) {
        game.over = true;
        game.won = true;
        flag_all_unrevealed();
    }
}

void flag_all_unrevealed() {
    const int rows = game.rows;
    const int columns = game.columns;

    for (int i = 0; i < columns; i++) {
        for (int j = 0; j < rows; j++) {
            if (game.grid[i][j].revealed || game.grid[i][j].flagged)
                continue;

            game.grid[i][j].flagged = true;
            game.flagged_mines++;
        }
    }
}

bool reveal_non_flagged(const int x, const int y, GridCoords *coords, int *counter) {
    const int rows = game.rows;
    const int columns = game.columns;
    bool revealed_mine = false;

    for (int i = -1; i <= 1; i++) {
        const int nx = x + i;

        if (nx < 0 || nx > columns - 1)
            continue;

        for (int j = -1; j <= 1; j++) {
            const int ny = y + j;

            if (ny < 0 || ny > rows - 1)
                continue;

            const auto [type, flagged, revealed] = game.grid[nx][ny];

            if (revealed || flagged)
                continue;

            coords[*counter] = {nx, ny};
            *counter = *counter + 1;

            if (type != CELL_MINE)
                game.unrevealed_count--;

            game.grid[nx][ny].revealed = true;

            if (type == CELL_MINE) {
                revealed_mine = true;
                break;
            }
        }

        if (revealed_mine)
            break;
    }

    return revealed_mine;
}

int count_surrounding_flagged(const int x, const int y) {
    const int rows = game.rows;
    const int columns = game.columns;

    int flagged = 0;
    for (int i = -1; i <= 1; i++) {
        const int nx = x + i;

        if (nx < 0 || nx > columns - 1)
            continue;

        for (int j = -1; j <= 1; j++) {
            const int ny = y + j;

            if (ny < 0 || ny > rows - 1 || game.grid[nx][ny].revealed || !game.grid[nx][ny].flagged)
                continue;

            flagged++;
        }
    }

    return flagged;
}

GridCoords get_surrounding_empty(const int x, const int y) {
    const int rows = game.rows;
    const int columns = game.columns;
    GridCoords coords = {-1, -1};

    for (int i = -1; i <= 1; i++) {
        const int nx = x + i;

        if (nx < 0 || nx > columns - 1)
            continue;

        bool found = false;

        for (int j = -1; j <= 1; j++) {
            const int ny = y + j;

            if (ny < 0 || ny > rows - 1 || game.grid[nx][ny].type != CELL_0 || game.grid[nx][ny].revealed)
                continue;

            coords.x = nx;
            coords.y = ny;
            found = true;
            break;
        }

        if (found)
            break;
    }

    return coords;
}

void reveal_cells_dfs(const int x, const int y) { // NOLINT(*-no-recursion)
    const int rows = game.rows;
    const int columns = game.columns;

    for (int i = -1; i <= 1; i++) {
        const int nx = x + i;

        if (nx < 0 || nx > columns - 1)
            continue;

        for (int j = -1; j <= 1; j++) {
            // Do not check corners
            if (abs(x) == 1 && abs(y) == 1)
                continue;

            const int ny = y + j;

            if (ny < 0 || ny > rows - 1 || game.grid[nx][ny].type != CELL_0 || game.grid[nx][ny].revealed)
                continue;

            game.grid[nx][ny].revealed = true;
            game.unrevealed_count--;
            reveal_cell_border(nx, ny);
            reveal_cells_dfs(nx, ny);
        }
    }
}

void reveal_cell_border(const int x, const int y) {
    const int rows = game.rows;
    const int columns = game.columns;

    for (int i = -1; i <= 1; i++) {
        const int bx = x + i;

        if (bx < 0 || bx > columns - 1)
            continue;

        for (int j = -1; j <= 1; j++) {
            const int by = y + j;
            const CellType cell_type = game.grid[bx][by].type;

            if (by < 0 || by > rows - 1 || game.grid[bx][by].revealed || cell_type == CELL_0 || cell_type == CELL_MINE)
                continue;

            game.grid[bx][by].revealed = true;
            game.unrevealed_count--;
        }
    }
}

CellType count_surrounding_mines(const int x, const int y) {
    const int rows = game.rows;
    const int columns = game.columns;
    int surrounding = CELL_0;

    for (int i = -1; i <= 1; i++) {
        const int nx = x + i;

        if (nx < 0 || nx > columns - 1)
            continue;

        for (int j = -1; j <= 1; j++) {
            const int ny = y + j;

            if (ny < 0 || ny > rows - 1)
                continue;

            if (game.grid[nx][ny].type == CELL_MINE)
                surrounding++;
        }
    }

    return static_cast<CellType>(surrounding);
}
