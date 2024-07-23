#pragma once

#include <ctime>

enum CELL_TYPE {
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

typedef CELL_TYPE CELL_TYPE;

typedef struct GridCell {
    CELL_TYPE type;
    bool flagged;
    bool revealed;
} GridCell;

typedef struct GridMeasurements {
    int cell_size;
    int cell_offset;
    int grid_line_length;
    int grid_line_width;
    int grid_x_offset;
    int grid_y_offset;
    int grid_width;
    int grid_height;
} GridMeasurements;

typedef struct Game_t {
    int rows;
    int columns;
    int total_mines;
    int flagged_mines;
    int unrevealed_count;
    time_t start_time;
    bool over;
    bool won;
    GridCell **grid;
    GridMeasurements measurements;
} Game_t;

extern Game_t game;

typedef struct GridCoords {
    int x;
    int y;
    bool inside;
} GridCoords;

void create_grid(int rows, int columns, int mines_count);
void place_grid_mines(int x, int y);
void calculate_grid_measurements();
void toggle_cell_flag(int x, int y);
void reveal_cell(int x, int y);
GridCoords calculate_grid_cell(int click_x, int click_y);
