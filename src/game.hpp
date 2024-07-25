#pragma once

#include <chrono>
#include <memory>
#include <SDL.h>
#include <thread>

#include "screens/screen.hpp"

struct GameParameters {
    SDL_Window *window;
    SDL_Renderer *renderer;
    long screen_refresh_rate;
};

class Game {
    using microseconds = std::chrono::microseconds;

    std::unique_ptr<Screen> m_screen{};
    microseconds m_render_interval_microsecs;
    SDL_Window *m_window;
    SDL_Renderer *m_renderer;

public:
    explicit Game(const GameParameters &parameters)
        : m_render_interval_microsecs(100000 / parameters.screen_refresh_rate),
          m_window(parameters.window),
          m_renderer(parameters.renderer) {}

    ~Game() = default;

    template <class ScreenT, typename... Args>
    void set_screen(Args... args) {
        m_screen = std::make_unique<ScreenT>(args...);
    }

    [[nodiscard]] SDL_Window *get_window() const {
        return m_window;
    }

    [[nodiscard]] SDL_Renderer *get_renderer() const {
        return m_renderer;
    }

    void run() const {
        SDL_Event event;
        bool quit = false;

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

            m_screen->render();

            std::this_thread::sleep_for(m_render_interval_microsecs);
        }
    }
};
