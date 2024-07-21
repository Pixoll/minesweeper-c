#include "textures.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>

#include "fonts.h"
#include "global.h"
#include "grid.h"
#include "util.h"

Texture gridTexture;

Texture cellMapTexture;
const int cellTextureSize = 512;

Texture cellCoveredTextures[TEXTURE_CELL_TYPES];
Texture cellCoveredMineTextures[TEXTURE_CELL_TYPES];
Texture cellFlaggedMineTextures[TEXTURE_CELL_TYPES];
Texture cellTriggeredMineTextures[TEXTURE_CELL_TYPES];
Texture cellFlagTextures[TEXTURE_CELL_TYPES];

Texture cellNumbersTextures[8];

Texture gameTimeTextTexture;
Texture remainingMinesTextTexture;
Texture remainingMinesIconTexture;

bool texturesReady = false;

const char *cellMapPath = "assets/images/cell_map.png";
const char *mineImagePath = "assets/images/mine.png";
const char *flagImagePath = "assets/images/flag.png";

const char *gridLineHorizontalImagePath = "assets/images/grid_line_horizontal.png";
const char *gridLineVerticalImagePath = "assets/images/grid_line_vertical.png";

const char *gridFillerHorizontalImagePath = "assets/images/grid_filler_horizontal.png";
const char *gridFillerVerticalImagePath = "assets/images/grid_filler_vertical.png";

const TEXTURE_CELL_TYPE textureCellSideTypeOrder[16] = {
    TEXTURE_CELL_NO_SIDES,
    TEXTURE_CELL_R,
    TEXTURE_CELL_L,
    TEXTURE_CELL_LR,
    TEXTURE_CELL_B,
    TEXTURE_CELL_BR,
    TEXTURE_CELL_BL,
    TEXTURE_CELL_BLR,
    TEXTURE_CELL_T,
    TEXTURE_CELL_TR,
    TEXTURE_CELL_TL,
    TEXTURE_CELL_TLR,
    TEXTURE_CELL_BT,
    TEXTURE_CELL_TBR,
    TEXTURE_CELL_TBL,
    TEXTURE_CELL_TBLR,
};

const TEXTURE_CELL_TYPE textureCellCornerTypeOrder[33] = {
    TEXTURE_CELL_TBLR_BRC,
    TEXTURE_CELL_TBLR_BLC,
    TEXTURE_CELL_TBLR_BLCRC,
    TEXTURE_CELL_TBLR_TRC,
    TEXTURE_CELL_TBLR_TRC_BRC,
    TEXTURE_CELL_TBLR_TRC_BLC,
    TEXTURE_CELL_TBLR_TRC_BLCRC,
    TEXTURE_CELL_TBLR_TLC,
    TEXTURE_CELL_TBLR_TLC_BRC,
    TEXTURE_CELL_TBLR_TLC_BLC,
    TEXTURE_CELL_TBLR_TLC_BLCRC,
    TEXTURE_CELL_TBLR_TLCRC,
    TEXTURE_CELL_TBLR_TLCRC_BRC,
    TEXTURE_CELL_TBLR_TLCRC_BLC,
    TEXTURE_CELL_TBLR_TLCRC_BLCRC,

    TEXTURE_CELL_BLR_BRC,
    TEXTURE_CELL_BLR_BLC,
    TEXTURE_CELL_BLR_BLCRC,
    TEXTURE_CELL_TBR_BRC,
    TEXTURE_CELL_TLR_TRC,
    TEXTURE_CELL_TBL_BLC,
    TEXTURE_CELL_TBR_TRC,
    TEXTURE_CELL_TBR_TRC_BRC,
    TEXTURE_CELL_TLR_TLC,
    0,
    0,
    TEXTURE_CELL_TBL_TLC,
    TEXTURE_CELL_TLR_TLCRC,
    TEXTURE_CELL_TBL_TLC_BLC,

    TEXTURE_CELL_BRC,
    TEXTURE_CELL_BLC,
    TEXTURE_CELL_TRC,
    TEXTURE_CELL_TLC,
};

void initTextureFromImage(const char *imagePath, Texture *destTexture) {
    SDL_Surface *surface = IMG_Load(imagePath);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    destTexture->area = rectangle(0, 0, surface->w, surface->h);
    destTexture->surface = surface;
    destTexture->texture = texture;
}

// void initCellSizedTextureFromImage(const char *imagePath, Texture *destTexture, const COLOR color) {
//     const int cellSize = game.measurements.cellSize;
//     const int textureSize = cellSize * 0.5;
//     const int gridLineWidth = game.measurements.gridLineWidth;
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

void initCellMapTexture() {
    SDL_Surface *surface = IMG_Load(cellMapPath);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    cellMapTexture.area = rectangle(0, 0, surface->w, surface->h);
    cellMapTexture.surface = surface;
    cellMapTexture.texture = texture;
}

void initCellTexturesFor(Texture textures[TEXTURE_CELL_TYPES], const char *imagePath, const float imageScaleWRTCell, const COLOR imageColor, const COLOR cellColor) {
    const int cellSize = game.measurements.cellSize;
    const int cellOffset = game.measurements.cellOffset;
    const int gridLineWidth = game.measurements.gridLineWidth;
    const SDL_Color cellTextureColor = colors[cellColor].rgb;

    for (TEXTURE_CELL_TYPE type = 0; type < TEXTURE_CELL_TYPES; type++) {
        SDL_Texture *finalTexture = createTexture(cellSize, cellSize, SDL_TEXTUREACCESS_TARGET);
        SDL_SetRenderTarget(renderer, finalTexture);
        SDL_SetTextureBlendMode(finalTexture, SDL_BLENDMODE_BLEND);

        const int mapIndex = type * cellTextureSize;
        const int mapX = mapIndex % cellMapTexture.area.w;
        const int mapY = (mapIndex / cellMapTexture.area.h) * cellTextureSize;
        const SDL_Rect cellTextureArea = rectangle(mapX, mapY, cellTextureSize, cellTextureSize);

        SDL_SetTextureColorMod(cellMapTexture.texture, cellTextureColor.r, cellTextureColor.g, cellTextureColor.b);
        SDL_RenderCopy(renderer, cellMapTexture.texture, &cellTextureArea, NULL);
        SDL_SetTextureColorMod(cellMapTexture.texture, 255, 255, 255);

        if (imagePath != NULL && imageScaleWRTCell != 0) {
            const int imageSize = cellSize * imageScaleWRTCell;
            const int imageOffset = (gridLineWidth + cellSize - imageSize) / 2 - cellOffset;
            const SDL_Color imageColorRgb = colors[imageColor].rgb;

            SDL_Surface *imageSurface = IMG_Load(imagePath);
            SDL_Texture *imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
            SDL_Rect imageArea = rectangle(imageOffset, imageOffset, imageSize, imageSize);

            SDL_SetTextureColorMod(imageTexture, imageColorRgb.r, imageColorRgb.g, imageColorRgb.b);
            SDL_RenderCopy(renderer, imageTexture, NULL, &imageArea);
        }

        SDL_Rect textureArea = rectangle(cellOffset, cellOffset, cellSize, cellSize);
        SDL_SetRenderTarget(renderer, NULL);

        textures[type].surface = NULL;
        textures[type].texture = finalTexture;
        textures[type].area = textureArea;
    }
}

void initCellNumbersTextures() {
    const int cellSize = game.measurements.cellSize;
    const int gridLineWidth = game.measurements.gridLineWidth;

    for (CELL_TYPE cell = CELL_1; cell <= CELL_8; cell++) {
        char cellText[2];
        snprintf(cellText, 2, "%c", '0' + cell - CELL_0);
        Color cellColor = colors[COLOR_GRID_1 + cell - 1];
        SDL_Surface *textSurface = TTF_RenderText_Solid(fontRubikMediumCellSized.font, cellText, cellColor.rgb);
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect cellArea;
        TTF_SizeText(fontRubikMediumCellSized.font, cellText, &cellArea.w, &cellArea.h);
        cellArea.x = (gridLineWidth + cellSize - cellArea.w) / 2;
        cellArea.y = (gridLineWidth + cellSize - cellArea.h) / 2;

        cellNumbersTextures[cell - CELL_1] = (Texture){.area = cellArea, .surface = textSurface, .texture = textTexture};
    }
}

void initGridTexture() {
    const int cellSize = game.measurements.cellSize;
    const int gridLineLength = game.measurements.gridLineLength;
    const int gridLineWidth = game.measurements.gridLineWidth;
    const int gridLineOffset = (gridLineWidth + cellSize - gridLineLength) / 2;
    const int gridXOffset = game.measurements.gridXOffset;
    const int gridYOffset = game.measurements.gridYOffset;
    const int gridWidth = game.measurements.gridWidth;
    const int gridHeight = game.measurements.gridHeight;
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

void initRemainingMinesIconTexture() {
    initTextureFromImage(mineImagePath, &remainingMinesIconTexture);
    const int size = fontRubikMediumPrimary.size;
    remainingMinesIconTexture.area.w = size;
    remainingMinesIconTexture.area.h = size;
}

void initTextures() {
    if (texturesReady) return;

    initGridTexture();

    initCellMapTexture();

    initCellTexturesFor(cellCoveredTextures, NULL, 0, 0, COLOR_THEME);
    initCellTexturesFor(cellFlagTextures, flagImagePath, 0.35, COLOR_FLAGGED_CELL, COLOR_FLAGGED_CELL_BG);
    initCellTexturesFor(cellFlaggedMineTextures, mineImagePath, 0.5, COLOR_FLAGGED_CELL, COLOR_FLAGGED_CELL_BG);
    initCellTexturesFor(cellCoveredMineTextures, mineImagePath, 0.5, COLOR_BACKGROUND, COLOR_THEME);
    initCellTexturesFor(cellTriggeredMineTextures, mineImagePath, 0.5, COLOR_TRIGGERED_MINE, COLOR_TRIGGERED_MINE_BG);

    initCellNumbersTextures();

    initRemainingMinesIconTexture();

    texturesReady = true;
}

void freeTexture(Texture texture) {
    SDL_FreeSurface(texture.surface);
    SDL_DestroyTexture(texture.texture);
}

void freeCellTexturesFrom(Texture textures[TEXTURE_CELL_TYPES]) {
    for (TEXTURE_CELL_TYPE type = 0; type < TEXTURE_CELL_TYPES; type++)
        freeTexture(textures[type]);
}

void freeCellNumbersTextures() {
    for (CELL_TYPE cell = CELL_1; cell <= CELL_8; cell++) {
        Texture cellTexture = cellNumbersTextures[cell - CELL_1];
        freeTexture(cellTexture);
    }
}

void freeTextures() {
    freeCellNumbersTextures();

    freeCellTexturesFrom(cellCoveredTextures);
    freeCellTexturesFrom(cellCoveredMineTextures);
    freeCellTexturesFrom(cellFlaggedMineTextures);
    freeCellTexturesFrom(cellTriggeredMineTextures);
    freeCellTexturesFrom(cellFlagTextures);

    freeTexture(cellMapTexture);

    freeTexture(gridTexture);

    freeTexture(gameTimeTextTexture);
    freeTexture(remainingMinesTextTexture);
}

void updateTextTexture(Texture *texture, Font font, COLOR color, const char *text) {
    if (texture->texture != NULL) freeTexture(*texture);

    SDL_Surface *textSurface = TTF_RenderText_Solid(font.font, text, colors[color].rgb);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect area = rectangle(0, 0, textSurface->w, textSurface->h);

    texture->surface = textSurface;
    texture->texture = textTexture;
    texture->area = area;
}
