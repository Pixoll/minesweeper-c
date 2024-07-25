#pragma once

#include <chrono>
#include <SDL.h>

#include "screens/screen.hpp"

struct GameParameters {
    SDL_Window *window;
    SDL_Renderer *renderer;
    long render_interval_microsecs;
};

class Game {
    Screen *m_screen = nullptr;
    Screen *m_last_screen = nullptr;
    long m_render_interval_microsecs;
    SDL_Window *m_window;
    SDL_Renderer *m_renderer;

public:
    explicit Game(const GameParameters &parameters)
        : m_render_interval_microsecs(parameters.render_interval_microsecs),
          m_window(parameters.window),
          m_renderer(parameters.renderer) {}

    ~Game() {
        delete m_last_screen;
        delete m_screen;
    }

    void set_screen(Screen *new_screen) {
        m_last_screen = m_screen;
        m_screen = new_screen;
    }

    [[nodiscard]] SDL_Window *get_window() const {
        return m_window;
    }

    [[nodiscard]] SDL_Renderer *get_renderer() const {
        return m_renderer;
    }

    void run() {
        using std::chrono::high_resolution_clock, std::chrono::duration_cast, std::chrono::microseconds;

        SDL_Event event;
        bool quit = false;
        high_resolution_clock::time_point last_render = high_resolution_clock::now();

        m_screen->render();

    game_loop:
        while (!quit) {
            while (SDL_PollEvent(&event) != 0) {
                switch (event.type) {
                    case SDL_QUIT:
                        quit = true;
                        goto game_loop;

                    default:
                        m_screen->run_logic(event);
                        break;
                }
            }

            const long microsecs_since_last_render = duration_cast<microseconds>(
                high_resolution_clock::now() - last_render
            ).count();

            if (microsecs_since_last_render >= m_render_interval_microsecs) {
                m_screen->render();
                last_render = high_resolution_clock::now();
            }

            if (m_last_screen) {
                delete m_last_screen;
                m_last_screen = nullptr;
            }
        }
    }
};
