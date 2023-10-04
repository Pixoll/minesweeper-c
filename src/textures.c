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

Texture cellCoveredMineTexture;
Texture cellFlaggedMineTexture;
Texture cellTriggeredMineTexture;
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

void initCellSizedTextureWithBgFromImage(const char *imagePath, Texture *destTexture, const COLOR imageColor, const COLOR bgColor) {
    const int cellSize = gridMeasurements.cellSize;
    const int gridLineWidth = gridMeasurements.gridLineWidth;
    const int backgroundSize = cellSize * 0.9;
    const int imageSize = cellSize * 0.5;
    const int backgroundOffset = (gridLineWidth + cellSize - backgroundSize) / 2;
    const int imageOffset = (gridLineWidth + cellSize - imageSize) / 2 - backgroundOffset;
    const SDL_Color imageColorRgb = colors[imageColor].rgb;

    const Uint32 pixelFormat = SDL_GetWindowSurface(window)->format->format;
    SDL_Texture *finalTexture = SDL_CreateTexture(renderer, pixelFormat, SDL_TEXTUREACCESS_TARGET, backgroundSize, backgroundSize);
    SDL_SetRenderTarget(renderer, finalTexture);

    SDL_Surface *backgroundSurface = createColoredSurface(backgroundSize, backgroundSize, bgColor);
    SDL_Texture *backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

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

    initCellSizedTextureWithBgFromImage(flagImagePath, &cellFlagTexture, COLOR_DARK_GREY, COLOR_LIGHT_GREY);
    initCellSizedTextureWithBgFromImage(mineImagePath, &cellFlaggedMineTexture, COLOR_DARK_GREY, COLOR_LIGHT_GREY);
    initCellSizedTextureWithBgFromImage(mineImagePath, &cellCoveredMineTexture, COLOR_DARK_GREY, COLOR_THEME);
    initCellSizedTextureWithBgFromImage(mineImagePath, &cellTriggeredMineTexture, COLOR_TRIGGERED_MINE, COLOR_TRIGGERED_MINE_BG);

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
    freeCoveredCellTexture();
    freeGridTexture();

    freeFlagTextures();
    freeMineTextures();
}
