#pragma once

#include "screen.hpp"
#include "../fonts.hpp"
#include "../grid.hpp"
#include "../grid_ui.hpp"
#include "../textures.hpp"

class Game;

class GameScreen final : virtual public Screen {
    struct WindowDimensions {
        int width = -1;
        int height = -1;
    };

    Game *m_game;
    bool m_placed_mines = false;
    SDL_Window *m_window;
    SDL_Renderer *m_renderer;
    WindowDimensions m_window_dimensions{};

public:
    explicit GameScreen(Game *game) : m_game(game) {
        m_window = m_game->get_window();
        m_renderer = m_game->get_renderer();

        create_grid(10, 20, 50);
        init_colors(m_window);

        const auto [r, g, b, a] = get_color(COLOR_BACKGROUND).rgb;
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);

        SDL_GetWindowSize(m_window, &m_window_dimensions.width, &m_window_dimensions.height);
        calculate_grid_measurements(m_window_dimensions.width, m_window_dimensions.height);

        init_fonts(m_window_dimensions.height);
        init_textures(m_renderer);
    }

    void run_logic(const SDL_Event &event) override {
        if (event.type != SDL_MOUSEBUTTONDOWN || get_game().over)
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
        SDL_RenderClear(m_renderer);
        draw_grid_ui(m_renderer);
        SDL_RenderPresent(m_renderer);
    }
};
