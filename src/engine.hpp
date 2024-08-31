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
    int screen_refresh_rate;
};

class Engine {
    std::unique_ptr<Screen> m_screen{};
    std::chrono::microseconds m_render_interval_microsecs;
    SDL_Window *m_window;
    SDL_Renderer *m_renderer;
    int m_window_width = 0;
    int m_window_height = 0;
    SDL_Color m_background_color{};

    SDL_Cursor *const m_arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);

public:
    explicit Engine(const EngineParameters &parameters)
        : m_render_interval_microsecs(100000 / parameters.screen_refresh_rate),
          m_window(parameters.window),
          m_renderer(parameters.renderer) {
        SDL_GetWindowSize(m_window, &m_window_width, &m_window_height);

        Color::make(m_window);
        Font::make_shared(m_window_height);

        m_background_color = Color::get(Color::BACKGROUND).get_rgb();
    }

    ~Engine() = default;

    template <class ScreenT, typename... Args>
    void set_screen(Args... args) {
        m_screen = std::make_unique<ScreenT>(args...);
        SDL_SetCursor(m_arrow_cursor);
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

        while (true) {
            while (SDL_PollEvent(&event) != 0) {
                m_screen->before_event(event);

                switch (event.type) {
                    case SDL_QUIT:
                        m_screen->on_quit_event(event.quit);
                        goto exit_game_loop;

                    case SDL_KEYDOWN:
                    case SDL_KEYUP:
                        m_screen->on_keyboard_event(event.key);
                        break;

                    case SDL_MOUSEBUTTONDOWN:
                    case SDL_MOUSEBUTTONUP:
                        m_screen->on_mouse_button_event(event.button);
                        break;

                    case SDL_MOUSEMOTION:
                        m_screen->on_mouse_motion_event(event.motion);
                        break;

                    case SDL_MOUSEWHEEL:
                        m_screen->on_mouse_wheel_event(event.wheel);
                        break;

                    default:
                        break;
                }
            }

            const auto [r, g, b, a] = m_background_color;
            SDL_SetRenderDrawColor(m_renderer, r, g, b, a);

            SDL_RenderClear(m_renderer);
            m_screen->render();
            SDL_RenderPresent(m_renderer);

            std::this_thread::sleep_for(m_render_interval_microsecs);
        }

    exit_game_loop:
        m_screen = nullptr;
        Font::free_shared();
    }
};
