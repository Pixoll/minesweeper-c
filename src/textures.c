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

SDL_Texture *gridTexture = NULL;

Texture cellNumbersTextures[8];
Texture cellCoveredTexture;

Texture gridFillerHorizontalTexture;
Texture gridFillerVerticalTexture;
Texture gridFillerCornerTexture;

Texture gridFlaggedFillerHorizontalTexture;
Texture gridFlaggedFillerVerticalTexture;
Texture gridFlaggedFillerCornerTexture;

Texture cellCoveredMineTexture;
Texture cellFlaggedMineTexture;
Texture cellTriggeredMineTexture;
Texture cellFlagTexture;

bool texturesReady = false;

const char *cellImagePath = "assets/images/cell.png";
const char *mineImagePath = "assets/images/mine.png";
const char *flagImagePath = "assets/images/flag.png";
const char *gridLineHorizontalImagePath = "assets/images/grid_line_horizontal.png";
const char *gridLineVerticalImagePath = "assets/images/grid_line_vertical.png";

enum FILLER_TYPE {
    FILLER_HORIZONTAL,
    FILLER_VERTICAL,
    FILLER_CORNER,
};
typedef enum FILLER_TYPE FILLER_TYPE;

void initCellCoveredTexture() {
    const int cellSize = gridMeasurements.cellSize;
    const int coveredCellSize = gridMeasurements.coveredCellSize;
    const int gridLineWidth = gridMeasurements.gridLineWidth;
    const int coveredCellOffset = (gridLineWidth + cellSize - coveredCellSize) / 2;
    const SDL_Color themeColor = colors[COLOR_THEME].rgb;

    SDL_Surface *surface = IMG_Load(cellImagePath);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect area = rectangle(coveredCellOffset, coveredCellOffset, coveredCellSize, coveredCellSize);

    SDL_SetTextureColorMod(texture, themeColor.r, themeColor.g, themeColor.b);

    cellCoveredTexture.area = area;
    cellCoveredTexture.surface = surface;
    cellCoveredTexture.texture = texture;
}

void initGridFillerTexture(FILLER_TYPE type, COLOR color, Texture *destTexture) {
    const int cellSize = gridMeasurements.cellSize;
    const int gridLineWidth = gridMeasurements.gridLineWidth;
    const int backgroundSize = gridMeasurements.coveredCellSize;
    const int fillerSize = cellSize - backgroundSize;
    const int fillerOppositeOffset = (gridLineWidth + fillerSize) / 2;
    const int fillerAdjacentOffset = fillerOppositeOffset + backgroundSize;

    SDL_Rect area = type == FILLER_HORIZONTAL ? rectangle(fillerOppositeOffset, fillerAdjacentOffset, backgroundSize, fillerSize)
                    : type == FILLER_VERTICAL ? rectangle(fillerAdjacentOffset, fillerOppositeOffset, fillerSize, backgroundSize)
                                              : rectangle(fillerAdjacentOffset, fillerAdjacentOffset, fillerSize, fillerSize);
    SDL_Surface *surface = createColoredSurface(area.w, area.h, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    destTexture->area = area;
    destTexture->surface = surface;
    destTexture->texture = texture;
}

// void initTextureFromImage(const char *imagePath, Texture *destTexture) {
//     SDL_Surface *surface = IMG_Load(imagePath);
//     SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
//     destTexture->area = rectangle(0, 0, surface->w, surface->h);
//     destTexture->surface = surface;
//     destTexture->texture = texture;
// }

// void initCellSizedTextureFromImage(const char *imagePath, Texture *destTexture, const COLOR color) {
//     const int cellSize = gridMeasurements.cellSize;
//     const int textureSize = cellSize * 0.5;
//     const int gridLineWidth = gridMeasurements.gridLineWidth;
//     const int textureOffset = (gridLineWidth + cellSize - textureSize) / 2;
//     const SDL_Color textureColor = colors[color].rgb;

//     SDL_Surface *surface = IMG_Load(imagePath);
//     SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
//     SDL_Rect area = rectangle(textureOffset, textureOffset, textureSize, textureSize);

//     SDL_SetTextureColorMod(texture, textureColor.r, textureColor.g, textureColor.b);

//     destTexture->area = area;
//     destTexture->surface = surface;
//     destTexture->texture = texture;
// }

void initCellSizedTextureWithBgFromImage(const char *imagePath, Texture *destTexture, const float imageScaleWRTCell, const COLOR imageColor, const COLOR cellColor) {
    const int cellSize = gridMeasurements.cellSize;
    const int gridLineWidth = gridMeasurements.gridLineWidth;
    const int backgroundSize = gridMeasurements.coveredCellSize;
    const int imageSize = cellSize * imageScaleWRTCell;
    const int backgroundOffset = (gridLineWidth + cellSize - backgroundSize) / 2;
    const int imageOffset = (gridLineWidth + cellSize - imageSize) / 2 - backgroundOffset;
    const SDL_Color imageColorRgb = colors[imageColor].rgb;
    const SDL_Color themeColor = colors[cellColor].rgb;

    SDL_Texture *finalTexture = createTexture(backgroundSize, backgroundSize, SDL_TEXTUREACCESS_TARGET);
    SDL_SetRenderTarget(renderer, finalTexture);

    SDL_Surface *backgroundSurface = createColoredSurface(backgroundSize, backgroundSize, COLOR_BACKGROUND);
    SDL_Texture *backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

    SDL_Surface *cellSurface = IMG_Load(cellImagePath);
    SDL_Texture *cellTexture = SDL_CreateTextureFromSurface(renderer, cellSurface);
    SDL_SetTextureColorMod(cellTexture, themeColor.r, themeColor.g, themeColor.b);
    SDL_RenderCopy(renderer, cellTexture, NULL, NULL);

    SDL_Surface *imageSurface = IMG_Load(imagePath);
    SDL_Texture *imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_Rect imageArea = rectangle(imageOffset, imageOffset, imageSize, imageSize);

    SDL_SetTextureColorMod(imageTexture, imageColorRgb.r, imageColorRgb.g, imageColorRgb.b);
    SDL_RenderCopy(renderer, imageTexture, NULL, &imageArea);

    SDL_Rect textureArea = rectangle(backgroundOffset, backgroundOffset, backgroundSize, backgroundSize);
    SDL_SetRenderTarget(renderer, NULL);

    destTexture->surface = NULL;
    destTexture->texture = finalTexture;
    destTexture->area = textureArea;
}

void initCellNumbersTextures() {
    const int cellSize = gridMeasurements.cellSize;
    const int gridLineWidth = gridMeasurements.gridLineWidth;

    for (CELL_TYPE cell = CELL_1; cell <= CELL_8; cell++) {
        char cellText[2];
        snprintf(cellText, 2, "%c", '0' + cell - CELL_0);
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
    const int gridLineLength = gridMeasurements.gridLineLength;
    const int gridLineWidth = gridMeasurements.gridLineWidth;
    const int gridLineOffset = (gridLineWidth + cellSize - gridLineLength) / 2;
    const int gridXOffset = gridMeasurements.gridXOffset;
    const int gridYOffset = gridMeasurements.gridYOffset;
    const int gridWidth = gridMeasurements.gridWidth;
    const int gridHeight = gridMeasurements.gridHeight;
    const SDL_Color colorGrid = colors[COLOR_LIGHT_GREY].rgb;

    gridTexture = createTexture(windowWidth, windowHeight, SDL_TEXTUREACCESS_TARGET);
    SDL_SetRenderTarget(renderer, gridTexture);

    SDL_Surface *backgroundSurface = createColoredSurface(windowWidth, windowHeight, COLOR_BACKGROUND);
    SDL_Texture *backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

    SDL_Surface *gridLineHSurface = IMG_Load(gridLineHorizontalImagePath);
    SDL_Texture *gridLineHTexture = SDL_CreateTextureFromSurface(renderer, gridLineHSurface);
    SDL_SetTextureColorMod(gridLineHTexture, colorGrid.r, colorGrid.g, colorGrid.b);

    SDL_Surface *gridLineVSurface = IMG_Load(gridLineVerticalImagePath);
    SDL_Texture *gridLineVTexture = SDL_CreateTextureFromSurface(renderer, gridLineVSurface);
    SDL_SetTextureColorMod(gridLineVTexture, colorGrid.r, colorGrid.g, colorGrid.b);

    for (int x = gridXOffset; x < gridXOffset + gridWidth - gridLineWidth; x += cellSize) {
        for (int y = gridYOffset; y < gridYOffset + gridHeight - gridLineWidth; y += cellSize) {
            if (x > gridXOffset) {
                const SDL_Rect vertical = rectangle(x, y + gridLineOffset, gridLineWidth, gridLineLength);
                SDL_RenderCopy(renderer, gridLineVTexture, NULL, &vertical);
            }
            if (y > gridYOffset) {
                const SDL_Rect horizontal = rectangle(x + gridLineOffset, y, gridLineLength, gridLineWidth);
                SDL_RenderCopy(renderer, gridLineHTexture, NULL, &horizontal);
            }
        }
    }

    SDL_SetRenderTarget(renderer, NULL);
}

void initTextures() {
    if (texturesReady) return;

    initCellSizedTextureWithBgFromImage(flagImagePath, &cellFlagTexture, 0.35, COLOR_FLAGGED_CELL, COLOR_FLAGGED_CELL_BG);
    initCellSizedTextureWithBgFromImage(mineImagePath, &cellFlaggedMineTexture, 0.5, COLOR_FLAGGED_CELL, COLOR_FLAGGED_CELL_BG);
    initCellSizedTextureWithBgFromImage(mineImagePath, &cellCoveredMineTexture, 0.5, COLOR_BACKGROUND, COLOR_THEME);
    initCellSizedTextureWithBgFromImage(mineImagePath, &cellTriggeredMineTexture, 0.5, COLOR_TRIGGERED_MINE, COLOR_TRIGGERED_MINE_BG);

    initGridFillerTexture(FILLER_HORIZONTAL, COLOR_THEME, &gridFillerHorizontalTexture);
    initGridFillerTexture(FILLER_VERTICAL, COLOR_THEME, &gridFillerVerticalTexture);
    initGridFillerTexture(FILLER_CORNER, COLOR_THEME, &gridFillerCornerTexture);

    initGridFillerTexture(FILLER_HORIZONTAL, COLOR_FLAGGED_CELL_BG, &gridFlaggedFillerHorizontalTexture);
    initGridFillerTexture(FILLER_VERTICAL, COLOR_FLAGGED_CELL_BG, &gridFlaggedFillerVerticalTexture);
    initGridFillerTexture(FILLER_CORNER, COLOR_FLAGGED_CELL_BG, &gridFlaggedFillerCornerTexture);

    initGridTexture();
    initCellCoveredTexture();
    initCellNumbersTextures();

    texturesReady = true;
}

void freeCellCoveredTexture() {
    SDL_FreeSurface(cellCoveredTexture.surface);
    SDL_DestroyTexture(cellCoveredTexture.texture);
}

void freeGridFillerTextures() {
    SDL_FreeSurface(gridFillerHorizontalTexture.surface);
    SDL_DestroyTexture(gridFillerHorizontalTexture.texture);
    SDL_FreeSurface(gridFillerVerticalTexture.surface);
    SDL_DestroyTexture(gridFillerVerticalTexture.texture);
    SDL_FreeSurface(gridFillerCornerTexture.surface);
    SDL_DestroyTexture(gridFillerCornerTexture.texture);

    SDL_FreeSurface(gridFlaggedFillerHorizontalTexture.surface);
    SDL_DestroyTexture(gridFlaggedFillerHorizontalTexture.texture);
    SDL_FreeSurface(gridFlaggedFillerVerticalTexture.surface);
    SDL_DestroyTexture(gridFlaggedFillerVerticalTexture.texture);
    SDL_FreeSurface(gridFlaggedFillerCornerTexture.surface);
    SDL_DestroyTexture(gridFlaggedFillerCornerTexture.texture);
}

void freeMineTextures() {
    SDL_FreeSurface(cellFlaggedMineTexture.surface);
    SDL_DestroyTexture(cellFlaggedMineTexture.texture);
    SDL_FreeSurface(cellCoveredMineTexture.surface);
    SDL_DestroyTexture(cellCoveredMineTexture.texture);
    SDL_FreeSurface(cellTriggeredMineTexture.surface);
    SDL_DestroyTexture(cellTriggeredMineTexture.texture);
}

void freeFlagTextures() {
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
    freeCellCoveredTexture();
    freeGridTexture();

    freeFlagTextures();
    freeMineTextures();
}
