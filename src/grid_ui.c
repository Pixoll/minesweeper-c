#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "grid.h"
#include "textures.h"
#include "util.h"

time_t lastGameTimeDrawn = 0;

void drawGrid(bool clickedMine);
void drawGameTime();

void drawGridUI(const bool clickedMine) {
    drawGrid(clickedMine);
    drawGameTime();
}

Texture getCellTexture(GridCell cell, bool clickedMine, TEXTURE_CELL_TYPE type);
TEXTURE_CELL_TYPE getCellType(int x, int y, bool flagged, bool revealed);

void drawGrid(const bool clickedMine) {
    const int rows = game.measurements.rows;
    const int columns = game.measurements.columns;
    const int cellSize = game.measurements.cellSize;
    const int gridXOffset = game.measurements.gridXOffset;
    const int gridYOffset = game.measurements.gridYOffset;

    // Draw grid
    SDL_RenderCopy(renderer, gridTexture.texture, NULL, &gridTexture.area);

    // Draw cells
    for (int i = 0; i < columns; i++) {
        const int x = gridXOffset + cellSize * i;
        for (int j = 0; j < rows; j++) {
            const int y = gridYOffset + cellSize * j;
            const GridCell cell = game.grid[i][j];
            if (cell.type == CELL_0 && cell.revealed) continue;

            TEXTURE_CELL_TYPE cellType = getCellType(i, j, cell.flagged, cell.revealed);
            Texture cellTexture = getCellTexture(cell, clickedMine, cellType);
            cellTexture.area.x += x;
            cellTexture.area.y += y;

            SDL_RenderCopy(renderer, cellTexture.texture, NULL, &cellTexture.area);
        }
    }
}

void drawGameTime() {
    if (game.startTime == 0) return;

    const time_t now = time(NULL);
    if (lastGameTimeDrawn < now) {
        lastGameTimeDrawn = now;
        char *timeString = getTimeString(now - game.startTime);
        updateGameTimeTexture(timeString);
        free(timeString);
    }

    gameTimeTexture.area.x = 10;
    gameTimeTexture.area.y = 10;
    SDL_RenderCopy(renderer, gameTimeTexture.texture, NULL, &gameTimeTexture.area);
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

bool verifyCell(const int x, const int y, const bool flagged) {
    return !game.grid[x][y].revealed && flagged == game.grid[x][y].flagged;
}

bool verifyCornersWithMask(const int corners, const int mask) {
    const bool bitsOutsideOfMask = corners & ~mask;
    const bool bitsInMask = corners & mask;
    return !bitsOutsideOfMask && bitsInMask;
}

TEXTURE_CELL_TYPE getCellType(const int x, const int y, const bool flagged, const bool revealed) {
    if (revealed) return TEXTURE_CELL_NO_SIDES;

    const int rows = game.measurements.rows;
    const int columns = game.measurements.columns;

    const bool T = y - 1 >= 0 && verifyCell(x, y - 1, flagged);
    const bool B = y + 1 <= rows - 1 && verifyCell(x, y + 1, flagged);
    const bool L = x - 1 >= 0 && verifyCell(x - 1, y, flagged);
    const bool R = x + 1 <= columns - 1 && verifyCell(x + 1, y, flagged);
    const bool TLC = T && L && verifyCell(x - 1, y - 1, flagged);
    const bool TRC = T && R && verifyCell(x + 1, y - 1, flagged);
    const bool BLC = B && L && verifyCell(x - 1, y + 1, flagged);
    const bool BRC = B && R && verifyCell(x + 1, y + 1, flagged);

    const int TBLR = T << 3 | B << 2 | L << 1 | R;
    const int TLR_BLR_C = TLC << 3 | TRC << 2 | BLC << 1 | BRC;

    if (TLR_BLR_C == 0b0000) return textureCellSideTypeOrder[TBLR];
    if (TBLR == 0b1111) return textureCellCornerTypeOrder[TLR_BLR_C - 1];

    if (TBLR == 0b0111 && verifyCornersWithMask(TLR_BLR_C, 0b0011)) return textureCellCornerTypeOrder[TLR_BLR_C + 14];
    if (TBLR == 0b1011 && verifyCornersWithMask(TLR_BLR_C, 0b1100)) return textureCellCornerTypeOrder[TLR_BLR_C + 15];
    if (TBLR == 0b1101 && verifyCornersWithMask(TLR_BLR_C, 0b0101)) return textureCellCornerTypeOrder[TLR_BLR_C + 17];
    if (TBLR == 0b1110 && verifyCornersWithMask(TLR_BLR_C, 0b1010)) return textureCellCornerTypeOrder[TLR_BLR_C + 18];

    if (isPow2(TLR_BLR_C)) return textureCellCornerTypeOrder[intLog2(TLR_BLR_C) + 29];

    // Impossible to reach? Not reached in huge grid
    printf("Not impossible to reach\n");

    return TEXTURE_CELL_NO_SIDES;
}
