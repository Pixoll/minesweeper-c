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

char **grid;
bool createdGrid = false;

GridMeasurements gridMeasurements;
bool gridMeasurementsReady = false;

GRID_CELL countSurroundingMines(int x, int y, int rows, int columns);

void createGrid(const int rows, const int columns, const int mines) {
    if (createdGrid) {
        for (int i = 0; i < columns; i++)
            free(grid[i]);
        free(grid);
    }

    grid = malloc(columns * sizeof(char *));
    for (int i = 0; i < columns; i++) {
        grid[i] = malloc(rows * sizeof(char));
        for (int j = 0; j < rows; j++)
            grid[i][j] = CELL_0;
    }

    srand(time(NULL));

    int placedMines = 0;
    // TODO This is bad
    while (placedMines < mines) {
        const int x = randomBetween(0, columns - 1);
        const int y = randomBetween(0, rows - 1);
        if (grid[x][y] == CELL_0) {
            grid[x][y] = CELL_MINE;
            placedMines++;
        }
    }

    // Count surrounding mines
    for (int i = 0; i < columns; i++) {
        for (int j = 0; j < rows; j++) {
            const GRID_CELL cell = grid[i][j];
            if (cell == CELL_MINE) continue;
            const GRID_CELL surrounding = countSurroundingMines(i, j, rows, columns);
            grid[i][j] = surrounding;
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

void drawGrid(const int rows, const int columns) {
    const int cellSize = gridMeasurements.cellSize;
    const int gridXOffset = gridMeasurements.gridXOffset;
    const int gridYOffset = gridMeasurements.gridYOffset;

    // Draw grid
    const SDL_Rect gridArea = rectangle(0, 0, windowWidth, windowHeight);
    SDL_RenderCopy(renderer, gridTexture, NULL, &gridArea);

    // Draw mines and numbers
    for (int i = 0; i < columns; i++) {
        const int x = gridXOffset + cellSize * i;
        for (int j = 0; j < rows; j++) {
            const int y = gridYOffset + cellSize * j;
            const GRID_CELL cell = grid[i][j];
            if (cell == CELL_0) continue;

            Texture cellTexture = cellTextures[cell];
            cellTexture.area.x += x;
            cellTexture.area.y += y;

            SDL_RenderCopy(renderer, cellTexture.texture, NULL, &cellTexture.area);
        }
    }
}

GRID_CELL countSurroundingMines(const int x, const int y, const int rows, const int columns) {
    int surrounding = 0;
    for (int i = -1; i <= 1; i++) {
        const int nx = x + i;
        if (nx < 0 || nx > columns - 1) continue;
        for (int j = -1; j <= 1; j++) {
            const int ny = y + j;
            if (ny < 0 || ny > rows - 1) continue;
            if (grid[nx][ny] == CELL_MINE) surrounding++;
        }
    }

    return surrounding;
}
