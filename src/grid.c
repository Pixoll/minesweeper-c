#include "grid.h"

#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "global.h"
#include "util.h"

typedef struct CellTexture {
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect area;
} CellTexture;

char **grid;
bool createdGrid = false;

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
        const int x = random(0, columns - 1);
        const int y = random(0, rows - 1);
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

void drawGrid(const int rows, const int columns) {
    const float gridRatio = (float)columns / rows;
    const float windowRatio = (float)windowWidth / windowHeight;

    const int limitantGridSide = gridRatio > windowRatio ? columns : rows;
    const int limitantWindowSide = gridRatio > windowRatio ? windowWidth : windowHeight;
    const int limitantOffset = limitantWindowSide * 0.025;
    const int cellSize = (limitantWindowSide - (limitantOffset << 1)) / limitantGridSide;

    const int gridLineWidth = cellSize * 0.1;
    const int gridWidth = cellSize * columns + gridLineWidth;
    const int gridXOffset = (windowWidth - gridWidth) / 2;
    const int gridHeight = cellSize * rows + gridLineWidth;
    const int gridYOffset = (windowHeight - gridHeight) / 2;

    SDL_Surface *surface = SDL_GetWindowSurface(window);

    // Draw grid
    for (int x = gridXOffset; x <= gridXOffset + gridWidth; x += cellSize) {
        const SDL_Rect gridLine = rectangle(x, gridYOffset, gridLineWidth, gridHeight);
        SDL_FillRect(surface, &gridLine, colors[COLOR_GREY].value);
    }
    for (int y = gridYOffset; y <= gridYOffset + gridHeight; y += cellSize) {
        const SDL_Rect gridLine = rectangle(gridXOffset, y, gridWidth, gridLineWidth);
        SDL_FillRect(surface, &gridLine, colors[COLOR_GREY].value);
    }

    SDL_Texture *gridTexture = SDL_CreateTextureFromSurface(renderer, surface);
    const SDL_Rect gridArea = rectangle(0, 0, windowWidth, windowHeight);
    SDL_RenderCopy(renderer, gridTexture, NULL, &gridArea);
    SDL_DestroyTexture(gridTexture);

    TTF_Font *RubikMedium = getFont(FONT_RUBIK_MEDIUM, cellSize);

    // Create cell textures
    CellTexture cellTextures[CELL_TYPES];
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

        cellTextures[cell] = (CellTexture){.area = cellArea, .surface = textSurface, .texture = textTexture};
        SDL_FreeSurface(textSurface);
    }

    // Draw mines and numbers
    for (int i = 0; i < columns; i++) {
        const int x = gridXOffset + cellSize * i;
        for (int j = 0; j < rows; j++) {
            const int y = gridYOffset + cellSize * j;
            const GRID_CELL cell = grid[i][j];
            if (cell == CELL_0) continue;

            CellTexture cellTexture = cellTextures[cell];
            cellTexture.area.x += x;
            cellTexture.area.y += y;

            SDL_RenderCopy(renderer, cellTexture.texture, NULL, &cellTexture.area);
        }
    }

    // Free cell textures
    for (GRID_CELL cell = CELL_1; cell < CELL_TYPES; cell++) {
        CellTexture cellTexture = cellTextures[cell];
        SDL_DestroyTexture(cellTexture.texture);
    }

    TTF_CloseFont(RubikMedium);
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
