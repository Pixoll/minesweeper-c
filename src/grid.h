#ifndef __GRID_DEFINED

#include <stdbool.h>
#include <time.h>

#define __GRID_DEFINED

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
typedef enum CELL_TYPE CELL_TYPE;

typedef struct GridCell {
    CELL_TYPE type;
    bool flagged;
    bool revealed;
} GridCell;

typedef struct GridMeasurements {
    int cellSize;
    int cellOffset;
    int gridLineLength;
    int gridLineWidth;
    int gridXOffset;
    int gridYOffset;
    int gridWidth;
    int gridHeight;
} GridMeasurements;

typedef struct Game {
    int rows;
    int columns;
    int totalMines;
    int flaggedMines;
    int unrevealedCount;
    time_t startTime;
    bool over;
    bool won;
    GridCell **grid;
    GridMeasurements measurements;
} Game;

extern Game game;

typedef struct Coords {
    int x;
    int y;
} Coords;

void createGrid(int rows, int columns, int minesCount);
void placeGridMines(int x, int y);
void calculateGridMeasurements();
void toggleCellFlag(int clickX, int clickY);
void revealCell(int clickX, int clickY);
Coords calculateGridCell(int clickX, int clickY);

#endif
