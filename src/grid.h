#pragma once

#include <stdbool.h>
#include <time.h>

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

typedef struct GridCoords {
    int x;
    int y;
    bool inside;
} GridCoords;

void createGrid(int rows, int columns, int minesCount);
void placeGridMines(int x, int y);
void calculateGridMeasurements();
void toggleCellFlag(int x, int y);
void revealCell(int x, int y);
GridCoords calculateGridCell(int clickX, int clickY);
