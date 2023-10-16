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

Texture gridTexture;

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

const char *gridFillerHorizontalImagePath = "assets/images/grid_filler_horizontal.png";
const char *gridFillerVerticalImagePath = "assets/images/grid_filler_vertical.png";

enum FILLER_TYPE {
    FILLER_HORIZONTAL,
    FILLER_VERTICAL,
    FILLER_CORNER,
};
typedef enum FILLER_TYPE FILLER_TYPE;

void initCellCoveredTexture() {
    const int cellSize = gridMeasurements.cellSize;
    const int gridLineWidth = gridMeasurements.gridLineWidth;
    const int cellOffset = gridMeasurements.cellOffset;
    const SDL_Color themeColor = colors[COLOR_THEME].rgb;

    SDL_Surface *surface = IMG_Load(cellImagePath);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect area = rectangle(cellOffset, cellOffset, cellSize, cellSize);

    SDL_SetTextureColorMod(texture, themeColor.r, themeColor.g, themeColor.b);

    cellCoveredTexture.area = area;
    cellCoveredTexture.surface = surface;
    cellCoveredTexture.texture = texture;
}

void initGridFillerTexture(FILLER_TYPE type, COLOR color, Texture *destTexture) {
    const int cellSize = gridMeasurements.cellSize;
    const int gridLineWidth = gridMeasurements.gridLineWidth;
    const int coveredCellSize = cellSize * 0.85;
    const int coveredCellOffset = (gridLineWidth + cellSize - coveredCellSize) / 2;
    const SDL_Color fillerColor = colors[color].rgb;

    SDL_Surface *surface = type == FILLER_HORIZONTAL ? IMG_Load(gridFillerHorizontalImagePath)
                           : type == FILLER_VERTICAL ? IMG_Load(gridFillerVerticalImagePath)
                                                     : createColoredSurface(coveredCellSize, coveredCellSize, COLOR_WHITE);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    const int fillerSize = type == FILLER_HORIZONTAL ? (float)surface->h * ((float)coveredCellSize / surface->w)
                           : type == FILLER_VERTICAL ? (float)surface->w * ((float)coveredCellSize / surface->h)
                                                     : cellSize - coveredCellSize;
    const int fillerOppositeOffset = type == FILLER_CORNER
                                         ? (gridLineWidth + cellSize + coveredCellSize) / 2
                                         : cellSize + (gridLineWidth - fillerSize) / 2;

    SDL_Rect area = type == FILLER_HORIZONTAL ? rectangle(coveredCellOffset, fillerOppositeOffset, coveredCellSize, fillerSize)
                    : type == FILLER_VERTICAL ? rectangle(fillerOppositeOffset, coveredCellOffset, fillerSize, coveredCellSize)
                                              : rectangle(fillerOppositeOffset, fillerOppositeOffset, fillerSize, fillerSize);

    SDL_SetTextureColorMod(texture, fillerColor.r, fillerColor.g, fillerColor.b);

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
    const int cellOffset = gridMeasurements.cellOffset;
    const int gridLineWidth = gridMeasurements.gridLineWidth;
    const int imageSize = cellSize * imageScaleWRTCell;
    const int imageOffset = (gridLineWidth + cellSize - imageSize) / 2 - cellOffset;
    const SDL_Color imageColorRgb = colors[imageColor].rgb;
    const SDL_Color themeColor = colors[cellColor].rgb;

    SDL_Texture *finalTexture = createTexture(cellSize, cellSize, SDL_TEXTUREACCESS_TARGET);
    SDL_SetRenderTarget(renderer, finalTexture);
    SDL_SetTextureBlendMode(finalTexture, SDL_BLENDMODE_BLEND);

    SDL_Surface *cellSurface = IMG_Load(cellImagePath);
    SDL_Texture *cellTexture = SDL_CreateTextureFromSurface(renderer, cellSurface);
    SDL_SetTextureColorMod(cellTexture, themeColor.r, themeColor.g, themeColor.b);
    SDL_RenderCopy(renderer, cellTexture, NULL, NULL);

    SDL_Surface *imageSurface = IMG_Load(imagePath);
    SDL_Texture *imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_Rect imageArea = rectangle(imageOffset, imageOffset, imageSize, imageSize);

    SDL_SetTextureColorMod(imageTexture, imageColorRgb.r, imageColorRgb.g, imageColorRgb.b);
    SDL_RenderCopy(renderer, imageTexture, NULL, &imageArea);

    SDL_Rect textureArea = rectangle(cellOffset, cellOffset, cellSize, cellSize);
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

    SDL_Texture *finalTexture = createTexture(gridWidth, gridHeight, SDL_TEXTUREACCESS_TARGET);
    SDL_SetRenderTarget(renderer, finalTexture);
    SDL_SetTextureBlendMode(finalTexture, SDL_BLENDMODE_BLEND);

    SDL_Surface *gridLineHSurface = IMG_Load(gridLineHorizontalImagePath);
    SDL_Texture *gridLineHTexture = SDL_CreateTextureFromSurface(renderer, gridLineHSurface);
    SDL_SetTextureColorMod(gridLineHTexture, colorGrid.r, colorGrid.g, colorGrid.b);

    SDL_Surface *gridLineVSurface = IMG_Load(gridLineVerticalImagePath);
    SDL_Texture *gridLineVTexture = SDL_CreateTextureFromSurface(renderer, gridLineVSurface);
    SDL_SetTextureColorMod(gridLineVTexture, colorGrid.r, colorGrid.g, colorGrid.b);

    for (int x = 0; x < gridWidth - gridLineWidth; x += cellSize) {
        for (int y = 0; y < gridHeight - gridLineWidth; y += cellSize) {
            if (x > 0) {
                const SDL_Rect vertical = rectangle(x, y + gridLineOffset, gridLineWidth, gridLineLength);
                SDL_RenderCopy(renderer, gridLineVTexture, NULL, &vertical);
            }
            if (y > 0) {
                const SDL_Rect horizontal = rectangle(x + gridLineOffset, y, gridLineLength, gridLineWidth);
                SDL_RenderCopy(renderer, gridLineHTexture, NULL, &horizontal);
            }
        }
    }

    SDL_SetRenderTarget(renderer, NULL);

    const SDL_Rect gridArea = rectangle(gridXOffset, gridYOffset, gridWidth, gridHeight);

    gridTexture.surface = NULL;
    gridTexture.texture = finalTexture;
    gridTexture.area = gridArea;
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

void freeTexture(Texture texture) {
    SDL_FreeSurface(texture.surface);
    SDL_DestroyTexture(texture.texture);
}

void freeCellCoveredTexture() {
    freeTexture(cellCoveredTexture);
}

void freeGridFillerTextures() {
    freeTexture(gridFillerHorizontalTexture);
    freeTexture(gridFillerVerticalTexture);
    freeTexture(gridFillerCornerTexture);

    freeTexture(gridFlaggedFillerHorizontalTexture);
    freeTexture(gridFlaggedFillerVerticalTexture);
    freeTexture(gridFlaggedFillerCornerTexture);
}

void freeMineTextures() {
    freeTexture(cellFlaggedMineTexture);
    freeTexture(cellCoveredMineTexture);
    freeTexture(cellTriggeredMineTexture);
}

void freeFlagTextures() {
    freeTexture(cellFlagTexture);
}

void freeCellNumbersTextures() {
    for (CELL_TYPE cell = CELL_1; cell <= CELL_8; cell++) {
        Texture cellTexture = cellNumbersTextures[cell - CELL_1];
        freeTexture(cellTexture);
    }
}

void freeTextures() {
    freeCellNumbersTextures();
    freeCellCoveredTexture();
    freeTexture(gridTexture);

    freeFlagTextures();
    freeMineTextures();
}
