#pragma once

#include <chrono>
#include <memory>
#include <SDL.h>
#include <thread>

#include "graphics/color.hpp"
#include "graphics/font.hpp"
#include "screens/screen.hpp"

struct EngineParameters {
    SDL_Window *window;
    SDL_Renderer *renderer;
    long screen_refresh_rate;
};

class Engine {
    std::unique_ptr<Screen> m_screen{};
    std::chrono::microseconds m_render_interval_microsecs;
    SDL_Window *m_window;
    SDL_Renderer *m_renderer;
    int m_window_width = 0;
    int m_window_height = 0;

public:
    explicit Engine(const EngineParameters &parameters)
        : m_render_interval_microsecs(100000 / parameters.screen_refresh_rate),
          m_window(parameters.window),
          m_renderer(parameters.renderer) {
        SDL_GetWindowSize(m_window, &m_window_width, &m_window_height);

        Color::init(m_window);
        Font::init_shared(m_window_height);

        const auto [r, g, b, a] = Color::get(Color::BACKGROUND).get_rgb();
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
    }

    ~Engine() = default;

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

    [[nodiscard]] int get_window_width() const {
        return m_window_width;
    }

    [[nodiscard]] int get_window_height() const {
        return m_window_height;
    }

    void run() {
        SDL_Event event;

        m_screen->render();

        while (true) {
            while (SDL_PollEvent(&event) != 0) {
                m_screen->run_logic(event);

                if (event.type == SDL_QUIT)
                    goto exit_game_loop;
            }

            m_screen->render();

            std::this_thread::sleep_for(m_render_interval_microsecs);
        }

    exit_game_loop:
        m_screen = nullptr;
        Font::free_shared();
    }
};
