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
    bool m_placed_mines = false;

public:
    explicit GameScreen(Game *game) : m_game(game) {
        window = m_game->get_window();
        renderer = m_game->get_renderer();

        create_grid(20, 20, 75);
        init_colors();

        const auto [r, g, b, a] = colors[COLOR_BACKGROUND].rgb;
        SDL_SetRenderDrawColor(renderer, r, g, b, a);

        get_window_size();
        calculate_grid_measurements();

        init_fonts();
        init_textures();
    }

    void run_logic(const SDL_Event &event) override {
        if (event.type != SDL_MOUSEBUTTONDOWN || game.over)
            return;

        int click_x, click_y;
        SDL_GetMouseState(&click_x, &click_y);
        const auto [x, y, inside] = calculate_grid_cell(click_x, click_y);

        if (!inside)
            return;

        switch (event.button.button) {
            case SDL_BUTTON_LEFT: {
                if (!m_placed_mines) {
                    place_grid_mines(x, y);
                    m_placed_mines = true;
                }

                reveal_cell(x, y);
                break;
            }

            case SDL_BUTTON_RIGHT: {
                if (!m_placed_mines)
                    break;

                toggle_cell_flag(x, y);
                break;
            }

            default:
                break;
        }
    }

    void render() override {
        get_window_size();
        calculate_grid_measurements();

        init_fonts();
        init_textures();

        SDL_RenderClear(renderer);

        draw_grid_ui();

        SDL_RenderPresent(renderer);
    }
};
