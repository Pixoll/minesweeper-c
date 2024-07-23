#pragma once

#include <ctime>
#include <vector>

enum CellType {
    CELL_0,
    CELL_1,
    CELL_2,
    CELL_3,
    CELL_4,
    CELL_5,
    CELL_6,
    CELL_7,
    CELL_8,
    CELL_MINE,
    CELL_TYPES,
};

struct GridCell {
    CellType type = CELL_0;
    bool flagged = false;
    bool revealed = false;
};

struct GridMeasurements {
    int cell_size = 0;
    int cell_offset = 0;
    int grid_line_length = 0;
    int grid_line_width = 0;
    int grid_x_offset = 0;
    int grid_y_offset = 0;
    int grid_width = 0;
    int grid_height = 0;
};

struct Game_t {
    int rows = 0;
    int columns = 0;
    int total_mines = 0;
    int flagged_mines = 0;
    int unrevealed_count = 0;
    time_t start_time = 0;
    bool over = false;
    bool won = false;
    std::vector<std::vector<GridCell>> grid;
    GridMeasurements measurements{};
};

extern Game_t game;

struct GridCoords {
    int x;
    int y;
    bool inside;
};

void create_grid(int rows, int columns, int mines_count);
void place_grid_mines(int x, int y);
void calculate_grid_measurements(int window_width, int window_height);
void toggle_cell_flag(int x, int y);
void reveal_cell(int x, int y);
GridCoords calculate_grid_cell(int click_x, int click_y);
