#pragma once

#include <SDL.h>

#include "game_screen.hpp"
#include "screen.hpp"
#include "../texture_managers/main_menu_texture_manager.hpp"

class Engine;

class MainMenuScreen final : virtual public Screen {
    Engine *m_engine;
    SDL_Renderer *m_renderer;
    int m_window_width;
    int m_window_height;
    MainMenuTextureManager m_texture_manager;

    SDL_Cursor *const m_arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    SDL_Cursor *const m_hand_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

public:
    explicit MainMenuScreen(Engine *engine) :
        m_engine(engine),
        m_renderer(engine->get_renderer()),
        m_window_width(engine->get_window_width()),
        m_window_height(engine->get_window_height()),
        m_texture_manager(m_renderer, m_window_width, m_window_height) {}

    ~MainMenuScreen() override = default;

    void run_logic(const SDL_Event &event) override {
        int click_x, click_y;
        SDL_GetMouseState(&click_x, &click_y);

        const bool cursor_in_new_game_button = m_texture_manager.get(MainMenuTextureManager::NEW_GAME_BUTTON)
                                                               ->contains(click_x, click_y);
        const bool cursor_in_continue_button = Game::save_exists()
                && m_texture_manager.get(MainMenuTextureManager::CONTINUE_GAME_BUTTON)->contains(click_x, click_y);

        SDL_SetCursor(cursor_in_new_game_button || cursor_in_continue_button ? m_hand_cursor : m_arrow_cursor);

        if (event.type != SDL_MOUSEBUTTONDOWN || event.button.button != SDL_BUTTON_LEFT)
            return;

        SDL_SetCursor(m_arrow_cursor);

        if (cursor_in_new_game_button) {
            m_engine->set_screen<GameScreen>(m_engine, 15, 20, 50);
        }

        if (cursor_in_continue_button) {
            m_engine->set_screen<GameScreen>(m_engine, Game::load());
        }
    }

    void render() override {
        SDL_RenderClear(m_renderer);

        m_texture_manager.get(MainMenuTextureManager::BIG_MINE)->render();
        m_texture_manager.get(MainMenuTextureManager::TITLE)->render();
        m_texture_manager.get(MainMenuTextureManager::NEW_GAME_BUTTON)->render();

        if (Game::save_exists())
            m_texture_manager.get(MainMenuTextureManager::CONTINUE_GAME_BUTTON)->render();

        SDL_RenderPresent(m_renderer);
    }
};
