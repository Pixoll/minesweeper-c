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

    SDL_Cursor *const m_arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    SDL_Cursor *const m_hand_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

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
        int click_x, click_y;
        SDL_GetMouseState(&click_x, &click_y);

        const bool cursor_in_new_game_button = get_main_menu_texture(MainMenuTexture::NEW_GAME_BUTTON)
               .contains(click_x, click_y);

        SDL_SetCursor(cursor_in_new_game_button ? m_hand_cursor : m_arrow_cursor);

        if (event.type != SDL_MOUSEBUTTONDOWN || event.button.button != SDL_BUTTON_LEFT)
            return;

        if (cursor_in_new_game_button) {
            m_engine->set_screen<GameScreen>(m_engine, 15, 20, 50);
            SDL_SetCursor(m_arrow_cursor);
        }
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
