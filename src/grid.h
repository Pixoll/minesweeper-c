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
    int cellSize;
    int gridLineWidth;
    int gridXOffset;
    int gridYOffset;
    int gridWidth;
    int gridHeight;
} GridMeasurements;

extern GridMeasurements gridMeasurements;

void createGrid(int rows, int columns, int mines);
void calculateGridMeasurements(int rows, int columns);
void drawGrid(int rows, int columns, bool lost);
bool revealCell(int rows, int columns, int clickX, int clickY, bool firstCell);

#endif
