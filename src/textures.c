#include "textures.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>

#include "fonts.h"
#include "global.h"
#include "grid.h"
#include "util.h"

Texture cellNumbersTextures[8];
SDL_Texture *gridTexture = NULL;
Texture coveredCellTexture;
Texture mineTexture;
Texture cellMineTexture;
Texture flagTexture;
Texture cellFlagTexture;
bool texturesReady = false;

const char *mineImagePath = "assets/images/mine.png";
const char *flagImagePath = "assets/images/flag.png";

void initCoveredCellTexture() {
    const int cellSize = gridMeasurements.cellSize;
    const int coveredCellSize = cellSize * 0.9;
    const int gridLineWidth = gridMeasurements.gridLineWidth;
    const int coveredCellOffset = (gridLineWidth + cellSize - coveredCellSize) / 2;

    SDL_Surface *surface = createColoredSurface(coveredCellSize, coveredCellSize, COLOR_THEME);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect area = rectangle(coveredCellOffset, coveredCellOffset, coveredCellSize, coveredCellSize);

    coveredCellTexture.area = area;
    coveredCellTexture.surface = surface;
    coveredCellTexture.texture = texture;
}

void initTextureFromImage(const char *imagePath, Texture *destTexture) {
    SDL_Surface *surface = IMG_Load(imagePath);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    destTexture->area = rectangle(0, 0, surface->w, surface->h);
    destTexture->surface = surface;
    destTexture->texture = texture;
}

void initCellSizedTextureFromImage(const char *imagePath, Texture *destTexture, const COLOR color) {
    const int cellSize = gridMeasurements.cellSize;
    const int textureSize = cellSize * 0.5;
    const int gridLineWidth = gridMeasurements.gridLineWidth;
    const int textureOffset = (gridLineWidth + cellSize - textureSize) / 2;
    const SDL_Color textureColor = colors[color].rgb;

    SDL_Surface *surface = IMG_Load(imagePath);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect area = rectangle(textureOffset, textureOffset, textureSize, textureSize);

    SDL_SetTextureColorMod(texture, textureColor.r, textureColor.g, textureColor.b);

    destTexture->area = area;
    destTexture->surface = surface;
    destTexture->texture = texture;
}

void initCellNumbersTextures() {
    const int cellSize = gridMeasurements.cellSize;
    const int gridLineWidth = gridMeasurements.gridLineWidth;

    for (CELL_TYPE cell = CELL_1; cell <= CELL_8; cell++) {
        char cellText[2];
        snprintf(cellText, 2, "%c", '0' + cell);
        Color cellColor = colors[COLOR_GRID_1 + cell - 1];
        SDL_Surface *textSurface = TTF_RenderText_Solid(fontRubikMediumCellSized, cellText, cellColor.rgb);
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect cellArea;
        TTF_SizeText(fontRubikMediumCellSized, cellText, &cellArea.w, &cellArea.h);
        cellArea.x = (gridLineWidth + cellSize - cellArea.w) / 2;
        cellArea.y = (gridLineWidth + cellSize - cellArea.h) / 2;

        cellNumbersTextures[cell - CELL_1] = (Texture){.area = cellArea, .surface = textSurface, .texture = textTexture};
    }
}

void initGridTexture() {
    const int cellSize = gridMeasurements.cellSize;
    const int gridLineLength = cellSize * 0.6;
    const int gridLineWidth = gridMeasurements.gridLineWidth;
    const int gridLineOffset = cellSize * 0.2 + gridLineWidth;
    const int gridXOffset = gridMeasurements.gridXOffset;
    const int gridYOffset = gridMeasurements.gridYOffset;
    const int gridWidth = gridMeasurements.gridWidth;
    const int gridHeight = gridMeasurements.gridHeight;

    SDL_Surface *surface = SDL_GetWindowSurface(window);

    const Uint32 colorGrid = colors[COLOR_LIGHT_GREY].value;
    const SDL_Rect background = rectangle(0, 0, windowWidth, windowHeight);
    SDL_FillRect(surface, &background, colors[COLOR_DARK_GREY].value);

    for (int x = gridXOffset; x < gridXOffset + gridWidth - gridLineWidth; x += cellSize) {
        for (int y = gridYOffset; y < gridYOffset + gridHeight - gridLineWidth; y += cellSize) {
            if (x > gridXOffset) {
                const SDL_Rect vertical = rectangle(x, y + gridLineOffset, gridLineWidth, gridLineLength);
                SDL_FillRect(surface, &vertical, colorGrid);
            }
            if (y > gridYOffset) {
                const SDL_Rect horizontal = rectangle(x + gridLineOffset, y, gridLineLength, gridLineWidth);
                SDL_FillRect(surface, &horizontal, colorGrid);
            }
        }
    }

    gridTexture = SDL_CreateTextureFromSurface(renderer, surface);
}

void initTextures() {
    if (texturesReady) return;

    initTextureFromImage(mineImagePath, &mineTexture);
    initTextureFromImage(flagImagePath, &flagTexture);

    // TODO Chance color to COLOR_DARK_GREY
    initCellSizedTextureFromImage(mineImagePath, &cellMineTexture, COLOR_WHITE);
    initCellSizedTextureFromImage(flagImagePath, &cellFlagTexture, COLOR_WHITE);

    initGridTexture();
    initCoveredCellTexture();
    initCellNumbersTextures();

    texturesReady = true;
}

void freeCoveredCellTexture() {
    SDL_FreeSurface(coveredCellTexture.surface);
    SDL_DestroyTexture(coveredCellTexture.texture);
}

void freeMineTextures() {
    SDL_FreeSurface(mineTexture.surface);
    SDL_DestroyTexture(mineTexture.texture);
    SDL_FreeSurface(cellMineTexture.surface);
    SDL_DestroyTexture(cellMineTexture.texture);
}

void freeFlagTextures() {
    SDL_FreeSurface(flagTexture.surface);
    SDL_DestroyTexture(flagTexture.texture);
    SDL_FreeSurface(cellFlagTexture.surface);
    SDL_DestroyTexture(cellFlagTexture.texture);
}

void freeCellNumbersTextures() {
    for (CELL_TYPE cell = CELL_1; cell <= CELL_8; cell++) {
        Texture cellTexture = cellNumbersTextures[cell - CELL_1];
        SDL_FreeSurface(cellTexture.surface);
        SDL_DestroyTexture(cellTexture.texture);
    }
}

void freeGridTexture() {
    SDL_DestroyTexture(gridTexture);
}

void freeTextures() {
    freeCellNumbersTextures();
    freeCoveredCellTexture();
    freeGridTexture();

    freeFlagTextures();
    freeMineTextures();
}
