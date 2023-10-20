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

Game game = {
    .rows = 0,
    .columns = 0,
    .totalMines = 0,
    .flaggedMines = 0,
    .startTime = 0,
};

bool createdGrid = false;
bool gridMeasurementsReady = false;

CELL_TYPE countSurroundingMines(int x, int y);

void createGrid(const int rows, const int columns, const int minesCount) {
    if (createdGrid) {
        for (int i = 0; i < columns; i++)
            free(game.grid[i]);
        free(game.grid);
    }

    game.grid = calloc(columns, sizeof(GridCell *));
    for (int i = 0; i < columns; i++) {
        game.grid[i] = calloc(rows, sizeof(GridCell));
        for (int j = 0; j < rows; j++) {
            game.grid[i][j] = (GridCell){
                .type = CELL_0,
                .flagged = false,
                .revealed = false,
            };
        }
    }

    game.rows = rows;
    game.columns = columns;
    game.totalMines = minesCount;
    createdGrid = true;
}

void placeGridMines(const int x, const int y) {
    const int rows = game.rows;
    const int columns = game.columns;
    const int count = game.totalMines;

    const time_t now = time(NULL);
    game.startTime = now;

    srand(now);

    int placedMines = 0;
    // TODO Is this bad???
    while (placedMines < count) {
        const int nx = randomBetween(0, columns - 1);
        const int ny = randomBetween(0, rows - 1);

        // Mines count at (x, y) must be 0
        if (nx >= x - 1 && nx <= x + 1 && ny >= y - 1 && ny <= y + 1) continue;

        if (game.grid[nx][ny].type == CELL_0) {
            game.grid[nx][ny].type = CELL_MINE;
            placedMines++;
        }
    }

    // Count surrounding mines
    for (int i = 0; i < columns; i++) {
        for (int j = 0; j < rows; j++) {
            const CELL_TYPE cell = game.grid[i][j].type;
            if (cell == CELL_MINE) continue;
            const CELL_TYPE surrounding = countSurroundingMines(i, j);
            game.grid[i][j].type = surrounding;
        }
    }
}

void calculateGridMeasurements() {
    if (gridMeasurementsReady) return;

    const int rows = game.rows;
    const int columns = game.columns;

    const float gridRatio = (float)columns / rows;
    const float windowRatio = (float)windowWidth / windowHeight;

    const int limitantGridSide = gridRatio > windowRatio ? columns : rows;
    const int limitantWindowSide = gridRatio > windowRatio ? windowWidth : windowHeight;
    const int limitantOffset = limitantWindowSide * 0.025;
    const int cellSize = (limitantWindowSide - (limitantOffset / 2)) / limitantGridSide;

    const int gridLineLength = cellSize * 0.65;
    const int gridLineWidth = cellSize * 0.03;
    const int gridWidth = cellSize * columns + gridLineWidth;
    const int gridXOffset = (windowWidth - gridWidth) / 2;
    const int gridHeight = cellSize * rows + gridLineWidth;
    const int gridYOffset = (windowHeight - gridHeight) / 2;

    const int cellOffset = gridLineWidth / 2;

    game.measurements.cellSize = cellSize;
    game.measurements.cellOffset = cellOffset;
    game.measurements.gridLineLength = gridLineLength;
    game.measurements.gridLineWidth = gridLineWidth;
    game.measurements.gridXOffset = gridXOffset;
    game.measurements.gridYOffset = gridYOffset;
    game.measurements.gridWidth = gridWidth;
    game.measurements.gridHeight = gridHeight;

    gridMeasurementsReady = true;
}

Coords calculateGridCell(const int clickX, const int clickY) {
    const int rows = game.rows;
    const int columns = game.columns;
    const int cellSize = game.measurements.cellSize;
    const int gridXOffset = game.measurements.gridXOffset;
    const int gridYOffset = game.measurements.gridYOffset;

    float x = (float)(clickX - gridXOffset) / cellSize;
    float y = (float)(clickY - gridYOffset) / cellSize;
    if (x < 0 || x > columns - 0.01f || y < 0 || y > rows - 0.01f) {
        x = -1;
        y = -1;
    }

    return (Coords){x, y};
}

void getSurroundingUnrevealed(int x, int y, Coords *coords, int *counter);

void toggleCellFlag(const int x, const int y) {
    if (x == -1 || y == -1) return;

    const int rows = game.rows;
    const int columns = game.columns;
    const GridCell cell = game.grid[x][y];
    if (!cell.revealed) {
        game.grid[x][y].flagged = !cell.flagged;

        if (!cell.flagged)
            game.flaggedMines++;
        else
            game.flaggedMines--;

        return;
    }

    if (cell.type < CELL_1 || cell.type > CELL_8) return;

    Coords unrevealed[9];
    int unrevealedCount = 0;
    getSurroundingUnrevealed(x, y, unrevealed, &unrevealedCount);

    if (unrevealedCount != cell.type - CELL_0) return;

    for (int i = 0; i < unrevealedCount; i++) {
        const int nx = unrevealed[i].x;
        const int ny = unrevealed[i].y;
        if (game.grid[nx][ny].flagged) continue;

        game.grid[nx][ny].flagged = true;
        game.flaggedMines++;
    }
}

void getSurroundingUnrevealed(const int x, const int y, Coords *coords, int *counter) {
    const int rows = game.rows;
    const int columns = game.columns;

    for (int i = -1; i <= 1; i++) {
        const int nx = x + i;
        if (nx < 0 || nx > columns - 1) continue;

        for (int j = -1; j <= 1; j++) {
            const int ny = y + j;
            if (ny < 0 || ny > rows - 1 || game.grid[nx][ny].revealed) continue;
            coords[*counter] = (Coords){nx, ny};
            *counter = *counter + 1;
        }
    }
}

int countSurroundingFlagged(int x, int y);
Coords getSurroundingEmpty(int x, int y);
bool revealNonFlagged(int x, int y, Coords *coords, int *counter);
void revealCellsDFS(int x, int y);
void revealCellBorder(int x, int y);

bool revealCell(int x, int y) {
    if (x == -1 || y == -1) return false;

    const int rows = game.rows;
    const int columns = game.columns;
    const CELL_TYPE cellType = game.grid[x][y].type;
    if (game.grid[x][y].flagged) return false;
    if (cellType == CELL_MINE) {
        game.grid[x][y].revealed = true;
        return true;
    }

    Coords revealedCells[9] = {{x, y}};
    int revealedCellsCount = 1;
    if (game.grid[x][y].revealed) {
        if (cellType < CELL_1 || cellType > CELL_8) return false;
        const int flaggedCount = countSurroundingFlagged(x, y);
        if (flaggedCount != cellType - CELL_0) return false;

        const bool revealedMine = revealNonFlagged(x, y, revealedCells, &revealedCellsCount);
        if (revealedMine) return revealedMine;
    }

    game.grid[x][y].revealed = true;
    for (int i = 0; i < revealedCellsCount; i++) {
        int nx = revealedCells[i].x;
        int ny = revealedCells[i].y;

        if (game.grid[nx][ny].type != CELL_0) continue;

        revealCellBorder(nx, ny);
        revealCellsDFS(nx, ny);
    }

    return false;
}

bool revealNonFlagged(const int x, const int y, Coords *coords, int *counter) {
    const int rows = game.rows;
    const int columns = game.columns;
    bool revealedMine;

    for (int i = -1; i <= 1; i++) {
        const int nx = x + i;
        if (nx < 0 || nx > columns - 1) continue;

        for (int j = -1; j <= 1; j++) {
            const int ny = y + j;
            if (ny < 0 || ny > rows - 1) continue;

            const GridCell cell = game.grid[nx][ny];
            if (cell.revealed || cell.flagged) continue;

            if (!cell.revealed) {
                coords[*counter] = (Coords){nx, ny};
                *counter = *counter + 1;
            }

            game.grid[nx][ny].revealed = true;

            if (cell.type == CELL_MINE) {
                revealedMine = true;
                break;
            }
        }

        if (revealedMine) break;
    }

    return revealedMine;
}

int countSurroundingFlagged(const int x, const int y) {
    const int rows = game.rows;
    const int columns = game.columns;

    int flagged = 0;
    for (int i = -1; i <= 1; i++) {
        const int nx = x + i;
        if (nx < 0 || nx > columns - 1) continue;

        for (int j = -1; j <= 1; j++) {
            const int ny = y + j;
            if (ny < 0 || ny > rows - 1 || game.grid[nx][ny].revealed || !game.grid[nx][ny].flagged) continue;
            flagged++;
        }
    }

    return flagged;
}

Coords getSurroundingEmpty(const int x, const int y) {
    const int rows = game.rows;
    const int columns = game.columns;
    Coords coords = {-1, -1};

    for (int i = -1; i <= 1; i++) {
        const int nx = x + i;
        if (nx < 0 || nx > columns - 1) continue;

        bool found = false;
        for (int j = -1; j <= 1; j++) {
            const int ny = y + j;
            if (ny < 0 || ny > rows - 1 || game.grid[nx][ny].type != CELL_0 || game.grid[nx][ny].revealed) continue;

            coords.x = nx;
            coords.y = ny;
            found = true;
            break;
        }

        if (found) break;
    }

    return coords;
}

void revealCellsDFS(const int x, const int y) {
    const int rows = game.rows;
    const int columns = game.columns;
    for (int i = -1; i <= 1; i++) {
        const int nx = x + i;
        if (nx < 0 || nx > columns - 1) continue;
        for (int j = -1; j <= 1; j++) {
            // Do not check corners
            if (abs(x) == 1 && abs(y) == 1) continue;

            const int ny = y + j;
            if (ny < 0 || ny > rows - 1 || game.grid[nx][ny].type != CELL_0 || game.grid[nx][ny].revealed) continue;

            game.grid[nx][ny].revealed = true;
            revealCellBorder(nx, ny);
            revealCellsDFS(nx, ny);
        }
    }
}

void revealCellBorder(const int x, const int y) {
    const int rows = game.rows;
    const int columns = game.columns;
    for (int i = -1; i <= 1; i++) {
        const int bx = x + i;
        if (bx < 0 || bx > columns - 1) continue;
        for (int j = -1; j <= 1; j++) {
            const int by = y + j;
            const CELL_TYPE cellType = game.grid[bx][by].type;
            if (by < 0 || by > rows - 1 || game.grid[bx][by].revealed || cellType == CELL_0 || cellType == CELL_MINE) continue;
            game.grid[bx][by].revealed = true;
        }
    }
}

CELL_TYPE countSurroundingMines(const int x, const int y) {
    const int rows = game.rows;
    const int columns = game.columns;
    CELL_TYPE surrounding = CELL_0;
    for (int i = -1; i <= 1; i++) {
        const int nx = x + i;
        if (nx < 0 || nx > columns - 1) continue;
        for (int j = -1; j <= 1; j++) {
            const int ny = y + j;
            if (ny < 0 || ny > rows - 1) continue;
            if (game.grid[nx][ny].type == CELL_MINE) surrounding++;
        }
    }

    return surrounding;
}
