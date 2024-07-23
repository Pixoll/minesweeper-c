#pragma once

#include "fonts.hpp"
#include "global.hpp"
#include "grid.hpp"
#include "grid_ui.hpp"
#include "screen.hpp"
#include "textures.hpp"

class Game;

class GameScreen final : virtual public Screen {
    Game *m_game;
    bool placedMines = false;

public:
    explicit GameScreen(Game *game) : m_game(game) {
        window = m_game->get_window();
        renderer = m_game->get_renderer();

        createGrid(20, 20, 75);
        initColors();

        const auto [r, g, b, a] = colors[COLOR_BACKGROUND].rgb;
        SDL_SetRenderDrawColor(renderer, r, g, b, a);

        getWindowSize();
        calculateGridMeasurements();

        initFonts();
        initTextures();
    }

    void run_logic(const SDL_Event &event) override {
        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN: {
                if (game.over)
                    break;

                int clickX, clickY;
                SDL_GetMouseState(&clickX, &clickY);
                const auto [x, y, inside] = calculateGridCell(clickX, clickY);

                switch (event.button.button) {
                    case SDL_BUTTON_LEFT: {
                        if (inside) {
                            if (!placedMines) {
                                placeGridMines(x, y);
                                placedMines = true;
                            }

                            revealCell(x, y);
                        }
                        break;
                    }
                    case SDL_BUTTON_RIGHT: {
                        if (inside) {
                            if (!placedMines)
                                break;
                            toggleCellFlag(x, y);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
            default:
                break;
        }
    }

    void render() override {
        getWindowSize();
        calculateGridMeasurements();

        initFonts();
        initTextures();

        SDL_RenderClear(renderer);

        drawGridUI();

        SDL_RenderPresent(renderer);
    }
};
