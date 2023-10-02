#include "textures.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>

#include "global.h"
#include "grid.h"
#include "util.h"

Texture cellTextures[CELL_TYPES];
SDL_Texture *gridTexture = NULL;
bool texturesReady = false;

void initCellTextures(TTF_Font *RubikMedium) {
    const int cellSize = gridMeasurements.cellSize;
    const int gridLineWidth = gridMeasurements.gridLineWidth;

    for (GRID_CELL cell = CELL_1; cell < CELL_TYPES; cell++) {
        char cellText[2];
        snprintf(cellText, 2, "%c", cell == CELL_MINE ? 'M' : ('0' + cell));
        Color cellColor = colors[cell == CELL_MINE ? COLOR_WHITE : (COLOR_GRID_1 + cell - 1)];
        SDL_Surface *textSurface = TTF_RenderText_Solid(RubikMedium, cellText, cellColor.rgb);
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect cellArea;
        TTF_SizeText(RubikMedium, cellText, &cellArea.w, &cellArea.h);
        cellArea.x = (gridLineWidth + cellSize - cellArea.w) / 2;
        cellArea.y = (gridLineWidth + cellSize - cellArea.h) / 2;

        cellTextures[cell] = (Texture){.area = cellArea, .surface = textSurface, .texture = textTexture};
    }
}

void initGridTexture() {
    const int cellSize = gridMeasurements.cellSize;
    const int gridLineWidth = gridMeasurements.gridLineWidth;
    const int gridXOffset = gridMeasurements.gridXOffset;
    const int gridYOffset = gridMeasurements.gridYOffset;
    const int gridWidth = gridMeasurements.gridWidth;
    const int gridHeight = gridMeasurements.gridHeight;

    SDL_Surface *surface = SDL_GetWindowSurface(window);
    for (int x = gridXOffset; x <= gridXOffset + gridWidth; x += cellSize) {
        const SDL_Rect gridLine = rectangle(x, gridYOffset, gridLineWidth, gridHeight);
        SDL_FillRect(surface, &gridLine, colors[COLOR_GREY].value);
    }
    for (int y = gridYOffset; y <= gridYOffset + gridHeight; y += cellSize) {
        const SDL_Rect gridLine = rectangle(gridXOffset, y, gridWidth, gridLineWidth);
        SDL_FillRect(surface, &gridLine, colors[COLOR_GREY].value);
    }

    gridTexture = SDL_CreateTextureFromSurface(renderer, surface);
}

void initTextures(TTF_Font *RubikMedium) {
    if (texturesReady) return;
    initGridTexture();
    initCellTextures(RubikMedium);
    texturesReady = true;
}

void freeCellTextures() {
    for (GRID_CELL cell = CELL_1; cell < CELL_TYPES; cell++) {
        Texture cellTexture = cellTextures[cell];
        SDL_FreeSurface(cellTexture.surface);
        SDL_DestroyTexture(cellTexture.texture);
    }
}

void freeGridTexture() {
    SDL_DestroyTexture(gridTexture);
}

void freeTextures() {
    freeCellTextures();
    freeGridTexture();
}
