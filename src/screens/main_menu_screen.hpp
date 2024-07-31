#pragma once

#include <SDL.h>

#include "game_screen.hpp"
#include "screen.hpp"
#include "../graphics/main_menu_textures.hpp"

class Engine;

class MainMenuScreen final : virtual public Screen {
    Engine *m_engine;
    SDL_Renderer *m_renderer;
    int m_window_width;
    int m_window_height;

public:
    explicit MainMenuScreen(Engine *engine) :
        m_engine(engine),
        m_renderer(engine->get_renderer()),
        m_window_width(engine->get_window_width()),
        m_window_height(engine->get_window_height()) {
        init_main_menu_fonts(m_window_height);
        init_main_menu_textures(m_renderer, m_window_width, m_window_height);
    }

    ~MainMenuScreen() override {
        free_main_menu_textures();
        free_main_menu_fonts();
    }

    void run_logic(const SDL_Event &event) override {
        if (event.type != SDL_MOUSEBUTTONDOWN)
            return;

        if (event.button.button != SDL_BUTTON_LEFT)
            return;

        m_engine->set_screen<GameScreen>(m_engine, 15, 20, 50);
    }

    void render() override {
        SDL_RenderClear(m_renderer);

        get_main_menu_texture(MainMenuTexture::BIG_MINE).render();
        get_main_menu_texture(MainMenuTexture::TITLE).render();
        get_main_menu_texture(MainMenuTexture::NEW_GAME_BUTTON).render();
        // get_main_menu_texture(MainMenuTexture::CONTINUE_GAME_BUTTON).render();

        SDL_RenderPresent(m_renderer);
    }
};
