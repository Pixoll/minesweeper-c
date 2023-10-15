#ifndef __GRID_DEFINED

#include <stdbool.h>

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
    int rows;
    int columns;
    int cellSize;
    int cellOffset;
    int gridLineLength;
    int gridLineWidth;
    int gridXOffset;
    int gridYOffset;
    int gridWidth;
    int gridHeight;
} GridMeasurements;

extern GridMeasurements gridMeasurements;

typedef struct Coords {
    int x;
    int y;
} Coords;

void createGrid(int rows, int columns);
void placeGridMines(int count, int x, int y);
void calculateGridMeasurements();
void drawGrid(bool clickedMine);
void toggleCellFlag(int clickX, int clickY);
bool revealCell(int clickX, int clickY);
Coords calculateGridCell(int clickX, int clickY);

#endif
