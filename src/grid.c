#include "grid.h"

#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "fonts.h"
#include "global.h"
#include "textures.h"
#include "util.h"

GridCell **grid;
bool createdGrid = false;

GridMeasurements gridMeasurements;
bool gridMeasurementsReady = false;

CELL_TYPE countSurroundingMines(int x, int y, int rows, int columns);

void createGrid(const int rows, const int columns, const int mines) {
    if (createdGrid) {
        for (int i = 0; i < columns; i++)
            free(grid[i]);
        free(grid);
    }

    grid = malloc(columns * sizeof(GridCell *));
    for (int i = 0; i < columns; i++) {
        grid[i] = malloc(rows * sizeof(GridCell));
        for (int j = 0; j < rows; j++) {
            grid[i][j] = (GridCell){
                .type = CELL_0,
                .flagged = false,
                .revealed = false,
            };
        }
    }

    srand(time(NULL));

    int placedMines = 0;
    // TODO This is bad
    while (placedMines < mines) {
        const int x = randomBetween(0, columns - 1);
        const int y = randomBetween(0, rows - 1);
        if (grid[x][y].type == CELL_0) {
            grid[x][y].type = CELL_MINE;
            placedMines++;
        }
    }

    // Count surrounding mines
    for (int i = 0; i < columns; i++) {
        for (int j = 0; j < rows; j++) {
            const CELL_TYPE cell = grid[i][j].type;
            if (cell == CELL_MINE) continue;
            const CELL_TYPE surrounding = countSurroundingMines(i, j, rows, columns);
            grid[i][j].type = surrounding;
        }
    }

    createdGrid = true;
}

void calculateGridMeasurements(const int rows, const int columns) {
    if (gridMeasurementsReady) return;

    const float gridRatio = (float)columns / rows;
    const float windowRatio = (float)windowWidth / windowHeight;

    const int limitantGridSide = gridRatio > windowRatio ? columns : rows;
    const int limitantWindowSide = gridRatio > windowRatio ? windowWidth : windowHeight;
    const int limitantOffset = limitantWindowSide * 0.025;
    const int cellSize = (limitantWindowSide - (limitantOffset << 1)) / limitantGridSide;

    const int gridLineWidth = cellSize * 0.025;
    const int gridWidth = cellSize * columns + gridLineWidth;
    const int gridXOffset = (windowWidth - gridWidth) / 2;
    const int gridHeight = cellSize * rows + gridLineWidth;
    const int gridYOffset = (windowHeight - gridHeight) / 2;

    gridMeasurements.cellSize = cellSize;
    gridMeasurements.gridLineWidth = gridLineWidth;
    gridMeasurements.gridXOffset = gridXOffset;
    gridMeasurements.gridYOffset = gridYOffset;
    gridMeasurements.gridWidth = gridWidth;
    gridMeasurements.gridHeight = gridHeight;

    gridMeasurementsReady = true;
}

void drawGrid(const int rows, const int columns, const bool lost) {
    const int cellSize = gridMeasurements.cellSize;
    const int gridXOffset = gridMeasurements.gridXOffset;
    const int gridYOffset = gridMeasurements.gridYOffset;

    // Draw grid
    const SDL_Rect gridArea = rectangle(0, 0, windowWidth, windowHeight);
    SDL_RenderCopy(renderer, gridTexture, NULL, &gridArea);

    // Draw cells
    for (int i = 0; i < columns; i++) {
        const int x = gridXOffset + cellSize * i;
        for (int j = 0; j < rows; j++) {
            const int y = gridYOffset + cellSize * j;
            const GridCell cell = grid[i][j];
            if (cell.type == CELL_0 && cell.revealed) continue;

            Texture cellTexture = lost && cell.type == CELL_MINE ? cellMineTexture
                                  : cell.flagged                 ? cellFlagTexture
                                  : !cell.revealed               ? coveredCellTexture
                                  : cell.type == CELL_MINE       ? cellMineTexture
                                                                 : cellNumbersTextures[cell.type - CELL_1];
            cellTexture.area.x += x;
            cellTexture.area.y += y;

            SDL_RenderCopy(renderer, cellTexture.texture, NULL, &cellTexture.area);
        }
    }
}

typedef struct Coords {
    int x;
    int y;
} Coords;

void calculateGridCell(const int clickX, const int clickY, int *x, int *y) {
    const int cellSize = gridMeasurements.cellSize;
    const int gridXOffset = gridMeasurements.gridXOffset;
    const int gridYOffset = gridMeasurements.gridYOffset;

    *x = (clickX - gridXOffset) / cellSize;
    *y = (clickY - gridYOffset) / cellSize;
}

void toggleCellFlag(const int clickX, const int clickY) {
    int x, y;
    calculateGridCell(clickX, clickY, &x, &y);
    grid[x][y].flagged = !grid[x][y].flagged;
}

void revealCellsDFS(int rows, int columns, int x, int y, Coords *group, int *groupSize);
void revealGroupBorderDFS(int rows, int columns, const Coords *group, int groupSize);

bool revealCell(const int rows, int const columns, const int clickX, const int clickY, const bool firstCell) {
    int x, y;
    calculateGridCell(clickX, clickY, &x, &y);
    const CELL_TYPE cellType = grid[x][y].type;

    if (grid[x][y].revealed || grid[x][y].flagged || (firstCell && cellType == CELL_MINE)) return false;

    grid[x][y].revealed = true;
    if (cellType != CELL_0) return cellType == CELL_MINE;

    Coords group[rows * columns];
    int groupSize = 0;
    revealCellsDFS(rows, columns, x, y, group, &groupSize);
    revealGroupBorderDFS(rows, columns, group, groupSize);

    return false;
}

void revealCellsDFS(const int rows, const int columns, const int x, const int y, Coords *group, int *groupSize) {
    for (int i = -1; i <= 1; i++) {
        const int nx = x + i;
        if (nx < 0 || nx > columns - 1) continue;
        for (int j = -1; j <= 1; j++) {
            // Do not check corners
            if (abs(x) == 1 && abs(y) == 1) continue;

            const int ny = y + j;
            if (ny < 0 || ny > rows - 1 || grid[nx][ny].type != CELL_0 || grid[nx][ny].revealed) continue;

            grid[nx][ny].revealed = true;

            group[*groupSize] = (Coords){nx, ny};
            const int incGroupSize = *groupSize + 1;
            *groupSize = incGroupSize;

            revealCellsDFS(rows, columns, nx, ny, group, groupSize);
        }
    }
}

void revealGroupBorderDFS(const int rows, const int columns, const Coords *group, const int groupSize) {
    for (int i = 0; i < groupSize; i++) {
        const int x = group[i].x;
        const int y = group[i].y;
        for (int i = -1; i <= 1; i++) {
            const int nx = x + i;
            if (nx < 0 || nx > columns - 1) continue;
            for (int j = -1; j <= 1; j++) {
                const int ny = y + j;
                if (ny < 0 || ny > rows - 1 || grid[nx][ny].revealed) continue;
                grid[nx][ny].revealed = true;
            }
        }
    }
}

CELL_TYPE countSurroundingMines(const int x, const int y, const int rows, const int columns) {
    int surrounding = 0;
    for (int i = -1; i <= 1; i++) {
        const int nx = x + i;
        if (nx < 0 || nx > columns - 1) continue;
        for (int j = -1; j <= 1; j++) {
            const int ny = y + j;
            if (ny < 0 || ny > rows - 1) continue;
            if (grid[nx][ny].type == CELL_MINE) surrounding++;
        }
    }

    return surrounding;
}
