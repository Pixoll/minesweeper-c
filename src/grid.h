#ifndef __GRID_DEFINED

#define __GRID_DEFINED

enum GRID_CELL {
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
typedef enum GRID_CELL GRID_CELL;

void createGrid(int rows, int columns, int mines);
void drawGrid(int rows, int columns);

#endif
