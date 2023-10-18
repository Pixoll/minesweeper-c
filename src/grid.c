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

CELL_TYPE countSurroundingMines(int x, int y);

void createGrid(const int rows, const int columns) {
    if (createdGrid) {
        for (int i = 0; i < columns; i++)
            free(grid[i]);
        free(grid);
    }

    grid = calloc(columns, sizeof(GridCell *));
    for (int i = 0; i < columns; i++) {
        grid[i] = calloc(rows, sizeof(GridCell));
        for (int j = 0; j < rows; j++) {
            grid[i][j] = (GridCell){
                .type = CELL_0,
                .flagged = false,
                .revealed = false,
            };
        }
    }

    gridMeasurements.rows = rows;
    gridMeasurements.columns = columns;
    createdGrid = true;
}

void placeGridMines(const int count, const int x, const int y) {
    const int rows = gridMeasurements.rows;
    const int columns = gridMeasurements.columns;

    srand(time(NULL));

    int placedMines = 0;
    // TODO Is this bad???
    while (placedMines < count) {
        const int nx = randomBetween(0, columns - 1);
        const int ny = randomBetween(0, rows - 1);

        // Mines count at (x, y) must be 0
        if (nx >= x - 1 && nx <= x + 1 && ny >= y - 1 && ny <= y + 1) continue;

        if (grid[nx][ny].type == CELL_0) {
            grid[nx][ny].type = CELL_MINE;
            placedMines++;
        }
    }

    // Count surrounding mines
    for (int i = 0; i < columns; i++) {
        for (int j = 0; j < rows; j++) {
            const CELL_TYPE cell = grid[i][j].type;
            if (cell == CELL_MINE) continue;
            const CELL_TYPE surrounding = countSurroundingMines(i, j);
            grid[i][j].type = surrounding;
        }
    }
}

void calculateGridMeasurements() {
    if (gridMeasurementsReady) return;

    const int rows = gridMeasurements.rows;
    const int columns = gridMeasurements.columns;

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

    gridMeasurements.cellSize = cellSize;
    gridMeasurements.cellOffset = cellOffset;
    gridMeasurements.gridLineLength = gridLineLength;
    gridMeasurements.gridLineWidth = gridLineWidth;
    gridMeasurements.gridXOffset = gridXOffset;
    gridMeasurements.gridYOffset = gridYOffset;
    gridMeasurements.gridWidth = gridWidth;
    gridMeasurements.gridHeight = gridHeight;

    gridMeasurementsReady = true;
}

Texture getCellTexture(GridCell cell, const bool clickedMine, TEXTURE_CELL_TYPE type) {
    if (clickedMine && cell.type == CELL_MINE) {
        if (cell.flagged) return cellFlaggedMineTextures[type];
        if (cell.revealed) return cellTriggeredMineTextures[type];
        return cellCoveredMineTextures[type];
    }
    if (cell.flagged) return cellFlagTextures[type];
    if (!cell.revealed) return cellCoveredTextures[type];
    return cellNumbersTextures[cell.type - CELL_1];
}

TEXTURE_CELL_TYPE getCellType(int x, int y, bool flagged, bool revealed);

void drawGrid(const bool clickedMine) {
    const int rows = gridMeasurements.rows;
    const int columns = gridMeasurements.columns;
    const int cellSize = gridMeasurements.cellSize;
    const int gridXOffset = gridMeasurements.gridXOffset;
    const int gridYOffset = gridMeasurements.gridYOffset;

    // Draw grid
    SDL_RenderCopy(renderer, gridTexture.texture, NULL, &gridTexture.area);

    // Draw cells
    for (int i = 0; i < columns; i++) {
        const int x = gridXOffset + cellSize * i;
        for (int j = 0; j < rows; j++) {
            const int y = gridYOffset + cellSize * j;
            const GridCell cell = grid[i][j];
            if (cell.type == CELL_0 && cell.revealed) continue;

            TEXTURE_CELL_TYPE cellType = getCellType(i, j, cell.flagged, cell.revealed);
            Texture cellTexture = getCellTexture(cell, clickedMine, cellType);
            cellTexture.area.x += x;
            cellTexture.area.y += y;

            SDL_RenderCopy(renderer, cellTexture.texture, NULL, &cellTexture.area);
        }
    }
}

TEXTURE_CELL_TYPE getCellType(const int x, const int y, const bool flagged, const bool revealed) {
    if (revealed) return TEXTURE_CELL_NO_SIDES;

    const int rows = gridMeasurements.rows;
    const int columns = gridMeasurements.columns;

    // TODO Proof of concept, make it more efficient

    const bool T = y - 1 >= 0 && !grid[x][y - 1].revealed && flagged == grid[x][y - 1].flagged;
    const bool B = y + 1 <= rows - 1 && !grid[x][y + 1].revealed && flagged == grid[x][y + 1].flagged;
    const bool L = x - 1 >= 0 && !grid[x - 1][y].revealed && flagged == grid[x - 1][y].flagged;
    const bool R = x + 1 <= columns - 1 && !grid[x + 1][y].revealed && flagged == grid[x + 1][y].flagged;
    const bool TLC = T && L && x - 1 >= 0 && y - 1 >= 0 && !grid[x - 1][y - 1].revealed && flagged == grid[x - 1][y - 1].flagged;
    const bool TRC = T && R && x + 1 <= columns - 1 && y - 1 >= 0 && !grid[x + 1][y - 1].revealed && flagged == grid[x + 1][y - 1].flagged;
    const bool BLC = B && L && x - 1 >= 0 && y + 1 <= rows - 1 && !grid[x - 1][y + 1].revealed && flagged == grid[x - 1][y + 1].flagged;
    const bool BRC = B && R && x + 1 <= columns - 1 && y + 1 <= rows - 1 && !grid[x + 1][y + 1].revealed && flagged == grid[x + 1][y + 1].flagged;

    if (!T && B && !L && !R && !TLC && !TRC && !BLC && !BRC) return TEXTURE_CELL_B;
    if (!T && B && L && !R && !TLC && !TRC && !BLC && !BRC) return TEXTURE_CELL_BL;
    if (!T && B && L && !R && !TLC && !TRC && BLC && !BRC) return TEXTURE_CELL_BLC;
    if (!T && B && L && R && !TLC && !TRC && !BLC && !BRC) return TEXTURE_CELL_BLR;
    if (!T && B && L && R && !TLC && !TRC && BLC && !BRC) return TEXTURE_CELL_BLR_BLC;
    if (!T && B && L && R && !TLC && !TRC && BLC && BRC) return TEXTURE_CELL_BLR_BLCRC;
    if (!T && B && L && R && !TLC && !TRC && !BLC && BRC) return TEXTURE_CELL_BLR_BRC;
    if (!T && B && !L && R && !TLC && !TRC && !BLC && !BRC) return TEXTURE_CELL_BR;
    if (!T && B && !L && R && !TLC && !TRC && !BLC && BRC) return TEXTURE_CELL_BRC;
    if (T && B && !L && !R && !TLC && !TRC && !BLC && !BRC) return TEXTURE_CELL_BT;
    if (!T && !B && L && !R && !TLC && !TRC && !BLC && !BRC) return TEXTURE_CELL_L;
    if (!T && !B && L && R && !TLC && !TRC && !BLC && !BRC) return TEXTURE_CELL_LR;
    if (!T && !B && !L && R && !TLC && !TRC && !BLC && !BRC) return TEXTURE_CELL_R;
    if (T && !B && !L && !R && !TLC && !TRC && !BLC && !BRC) return TEXTURE_CELL_T;
    if (T && B && L && !R && !TLC && !TRC && !BLC && !BRC) return TEXTURE_CELL_TBL;
    if (T && B && L && R && !TLC && !TRC && !BLC && !BRC) return TEXTURE_CELL_TBLR;
    if (T && B && L && R && !TLC && !TRC && BLC && !BRC) return TEXTURE_CELL_TBLR_BLC;
    if (T && B && L && R && !TLC && !TRC && BLC && BRC) return TEXTURE_CELL_TBLR_BLCRC;
    if (T && B && L && R && !TLC && !TRC && !BLC && BRC) return TEXTURE_CELL_TBLR_BRC;
    if (T && B && L && R && TLC && !TRC && !BLC && !BRC) return TEXTURE_CELL_TBLR_TLC;
    if (T && B && L && R && TLC && TRC && !BLC && !BRC) return TEXTURE_CELL_TBLR_TLCRC;
    if (T && B && L && R && TLC && TRC && BLC && !BRC) return TEXTURE_CELL_TBLR_TLCRC_BLC;
    if (T && B && L && R && TLC && TRC && BLC && BRC) return TEXTURE_CELL_TBLR_TLCRC_BLCRC;
    if (T && B && L && R && TLC && TRC && !BLC && BRC) return TEXTURE_CELL_TBLR_TLCRC_BRC;
    if (T && B && L && R && TLC && !TRC && BLC && !BRC) return TEXTURE_CELL_TBLR_TLC_BLC;
    if (T && B && L && R && TLC && !TRC && BLC && BRC) return TEXTURE_CELL_TBLR_TLC_BLCRC;
    if (T && B && L && R && TLC && !TRC && !BLC && BRC) return TEXTURE_CELL_TBLR_TLC_BRC;
    if (T && B && L && R && !TLC && TRC && !BLC && !BRC) return TEXTURE_CELL_TBLR_TRC;
    if (T && B && L && R && !TLC && TRC && BLC && !BRC) return TEXTURE_CELL_TBLR_TRC_BLC;
    if (T && B && L && R && !TLC && TRC && BLC && BRC) return TEXTURE_CELL_TBLR_TRC_BLCRC;
    if (T && B && L && R && !TLC && TRC && !BLC && BRC) return TEXTURE_CELL_TBLR_TRC_BRC;
    if (T && B && L && !R && !TLC && !TRC && BLC && !BRC) return TEXTURE_CELL_TBL_BLC;
    if (T && B && L && !R && TLC && !TRC && !BLC && !BRC) return TEXTURE_CELL_TBL_TLC;
    if (T && B && L && !R && TLC && !TRC && BLC && !BRC) return TEXTURE_CELL_TBL_TLC_BLC;
    if (T && B && !L && R && !TLC && !TRC && !BLC && !BRC) return TEXTURE_CELL_TBR;
    if (T && B && !L && R && !TLC && !TRC && !BLC && BRC) return TEXTURE_CELL_TBR_BRC;
    if (T && B && !L && R && !TLC && TRC && !BLC && !BRC) return TEXTURE_CELL_TBR_TRC;
    if (T && B && !L && R && !TLC && TRC && !BLC && BRC) return TEXTURE_CELL_TBR_TRC_BRC;
    if (T && !B && L && !R && !TLC && !TRC && !BLC && !BRC) return TEXTURE_CELL_TL;
    if (T && !B && L && !R && TLC && !TRC && !BLC && !BRC) return TEXTURE_CELL_TLC;
    if (T && !B && L && R && !TLC && !TRC && !BLC && !BRC) return TEXTURE_CELL_TLR;
    if (T && !B && L && R && TLC && !TRC && !BLC && !BRC) return TEXTURE_CELL_TLR_TLC;
    if (T && !B && L && R && TLC && TRC && !BLC && !BRC) return TEXTURE_CELL_TLR_TLCRC;
    if (T && !B && L && R && !TLC && TRC && !BLC && !BRC) return TEXTURE_CELL_TLR_TRC;
    if (T && !B && !L && R && !TLC && !TRC && !BLC && !BRC) return TEXTURE_CELL_TR;
    if (T && !B && !L && R && !TLC && TRC && !BLC && !BRC) return TEXTURE_CELL_TRC;

    return TEXTURE_CELL_NO_SIDES;
}

Coords calculateGridCell(const int clickX, const int clickY) {
    const int rows = gridMeasurements.rows;
    const int columns = gridMeasurements.columns;
    const int cellSize = gridMeasurements.cellSize;
    const int gridXOffset = gridMeasurements.gridXOffset;
    const int gridYOffset = gridMeasurements.gridYOffset;

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

    const int rows = gridMeasurements.rows;
    const int columns = gridMeasurements.columns;
    const GridCell cell = grid[x][y];
    if (!cell.revealed) {
        grid[x][y].flagged = !grid[x][y].flagged;
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
        grid[nx][ny].flagged = true;
    }
}

void getSurroundingUnrevealed(const int x, const int y, Coords *coords, int *counter) {
    const int rows = gridMeasurements.rows;
    const int columns = gridMeasurements.columns;

    for (int i = -1; i <= 1; i++) {
        const int nx = x + i;
        if (nx < 0 || nx > columns - 1) continue;

        for (int j = -1; j <= 1; j++) {
            const int ny = y + j;
            if (ny < 0 || ny > rows - 1 || grid[nx][ny].revealed) continue;
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

    const int rows = gridMeasurements.rows;
    const int columns = gridMeasurements.columns;
    const CELL_TYPE cellType = grid[x][y].type;
    if (grid[x][y].flagged) return false;
    if (cellType == CELL_MINE) {
        grid[x][y].revealed = true;
        return true;
    }

    Coords revealedCells[9] = {{x, y}};
    int revealedCellsCount = 1;
    if (grid[x][y].revealed) {
        if (cellType < CELL_1 || cellType > CELL_8) return false;
        const int flaggedCount = countSurroundingFlagged(x, y);
        if (flaggedCount != cellType - CELL_0) return false;

        const bool revealedMine = revealNonFlagged(x, y, revealedCells, &revealedCellsCount);
        if (revealedMine) return revealedMine;
    }

    grid[x][y].revealed = true;
    for (int i = 0; i < revealedCellsCount; i++) {
        int nx = revealedCells[i].x;
        int ny = revealedCells[i].y;

        if (grid[nx][ny].type != CELL_0) continue;

        revealCellBorder(nx, ny);
        revealCellsDFS(nx, ny);
    }

    return false;
}

bool revealNonFlagged(const int x, const int y, Coords *coords, int *counter) {
    const int rows = gridMeasurements.rows;
    const int columns = gridMeasurements.columns;
    bool revealedMine;

    for (int i = -1; i <= 1; i++) {
        const int nx = x + i;
        if (nx < 0 || nx > columns - 1) continue;

        for (int j = -1; j <= 1; j++) {
            const int ny = y + j;
            if (ny < 0 || ny > rows - 1) continue;

            const GridCell cell = grid[nx][ny];
            if (cell.revealed || cell.flagged) continue;

            if (!cell.revealed) {
                coords[*counter] = (Coords){nx, ny};
                *counter = *counter + 1;
            }

            grid[nx][ny].revealed = true;

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
    const int rows = gridMeasurements.rows;
    const int columns = gridMeasurements.columns;

    int flagged = 0;
    for (int i = -1; i <= 1; i++) {
        const int nx = x + i;
        if (nx < 0 || nx > columns - 1) continue;

        for (int j = -1; j <= 1; j++) {
            const int ny = y + j;
            if (ny < 0 || ny > rows - 1 || grid[nx][ny].revealed || !grid[nx][ny].flagged) continue;
            flagged++;
        }
    }

    return flagged;
}

Coords getSurroundingEmpty(const int x, const int y) {
    const int rows = gridMeasurements.rows;
    const int columns = gridMeasurements.columns;
    Coords coords = {-1, -1};

    for (int i = -1; i <= 1; i++) {
        const int nx = x + i;
        if (nx < 0 || nx > columns - 1) continue;

        bool found = false;
        for (int j = -1; j <= 1; j++) {
            const int ny = y + j;
            if (ny < 0 || ny > rows - 1 || grid[nx][ny].type != CELL_0 || grid[nx][ny].revealed) continue;

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
    const int rows = gridMeasurements.rows;
    const int columns = gridMeasurements.columns;
    for (int i = -1; i <= 1; i++) {
        const int nx = x + i;
        if (nx < 0 || nx > columns - 1) continue;
        for (int j = -1; j <= 1; j++) {
            // Do not check corners
            if (abs(x) == 1 && abs(y) == 1) continue;

            const int ny = y + j;
            if (ny < 0 || ny > rows - 1 || grid[nx][ny].type != CELL_0 || grid[nx][ny].revealed) continue;

            grid[nx][ny].revealed = true;
            revealCellBorder(nx, ny);
            revealCellsDFS(nx, ny);
        }
    }
}

void revealCellBorder(const int x, const int y) {
    const int rows = gridMeasurements.rows;
    const int columns = gridMeasurements.columns;
    for (int i = -1; i <= 1; i++) {
        const int bx = x + i;
        if (bx < 0 || bx > columns - 1) continue;
        for (int j = -1; j <= 1; j++) {
            const int by = y + j;
            const CELL_TYPE cellType = grid[bx][by].type;
            if (by < 0 || by > rows - 1 || grid[bx][by].revealed || cellType == CELL_0 || cellType == CELL_MINE) continue;
            grid[bx][by].revealed = true;
        }
    }
}

CELL_TYPE countSurroundingMines(const int x, const int y) {
    const int rows = gridMeasurements.rows;
    const int columns = gridMeasurements.columns;
    CELL_TYPE surrounding = CELL_0;
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
