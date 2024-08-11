#pragma once

#include <SDL.h>

#include "game_screen.hpp"
#include "screen.hpp"
#include "../texture_managers/main_menu_texture_manager.hpp"

class Engine;

class MainMenuScreen final : virtual public Screen {
    using TextureName = MainMenuTextureManager::TextureName;

    Engine *m_engine;
    SDL_Renderer *m_renderer;
    int m_window_width;
    int m_window_height;
    MainMenuTextureManager m_texture_manager;
    static Game::Difficulty selected_difficulty;

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
        SDL_Point cursor_pos;
        SDL_GetMouseState(&cursor_pos.x, &cursor_pos.y);

        const bool cursor_in_quit_button = m_texture_manager.get(TextureName::QUIT_BUTTON)->contains(cursor_pos);

        const bool cursor_in_new_game_button = m_texture_manager.get(TextureName::NEW_GAME_BUTTON)
                                                               ->contains(cursor_pos);

        const bool cursor_in_continue_button = Game::save_exists(selected_difficulty)
                && m_texture_manager.get(TextureName::CONTINUE_GAME_BUTTON)->contains(cursor_pos);

        const bool cursor_in_left_arrow = selected_difficulty != Game::DIFFIC_LOWEST
                && m_texture_manager.get(TextureName::LEFT_ARROW)->contains(cursor_pos);

        const bool cursor_in_right_arrow = selected_difficulty != Game::DIFFIC_HIGHEST
                && m_texture_manager.get(TextureName::RIGHT_ARROW)->contains(cursor_pos);

        SDL_SetCursor(
            cursor_in_quit_button || cursor_in_new_game_button || cursor_in_continue_button || cursor_in_left_arrow
            || cursor_in_right_arrow
            ? m_hand_cursor
            : m_arrow_cursor
        );

        if (event.type != SDL_MOUSEBUTTONDOWN || event.button.button != SDL_BUTTON_LEFT)
            return;

        if (cursor_in_quit_button) {
            SDL_Event quit_event = {.type = SDL_QUIT};
            SDL_PushEvent(&quit_event);
            return;
        }

        if (cursor_in_new_game_button) {
            m_engine->set_screen<GameScreen>(m_engine, selected_difficulty);
            SDL_SetCursor(m_arrow_cursor);
            return;
        }

        if (cursor_in_continue_button) {
            m_engine->set_screen<GameScreen>(m_engine, Game::load(selected_difficulty));
            SDL_SetCursor(m_arrow_cursor);
            return;
        }

        if (cursor_in_left_arrow) {
            selected_difficulty = static_cast<Game::Difficulty>(selected_difficulty - 1);
            if (selected_difficulty == Game::DIFFIC_LOWEST)
                SDL_SetCursor(m_arrow_cursor);
            return;
        }

        if (cursor_in_right_arrow) {
            selected_difficulty = static_cast<Game::Difficulty>(selected_difficulty + 1);
            if (selected_difficulty == Game::DIFFIC_HIGHEST)
                SDL_SetCursor(m_arrow_cursor);
            return;
        }
    }

    void render() override {
        m_texture_manager.get(TextureName::BIG_MINE)->render();
        m_texture_manager.get(TextureName::TITLE)->render();
        m_texture_manager.get(TextureName::QUIT_BUTTON)->render();
        m_texture_manager.get(TextureName::NEW_GAME_BUTTON)->render();

        if (Game::save_exists(selected_difficulty))
            m_texture_manager.get(TextureName::CONTINUE_GAME_BUTTON)->render();

        if (selected_difficulty != Game::DIFFIC_LOWEST)
            m_texture_manager.get(TextureName::LEFT_ARROW)->render();

        if (selected_difficulty != Game::DIFFIC_HIGHEST)
            m_texture_manager.get(TextureName::RIGHT_ARROW)->render();

        m_texture_manager.get(selected_difficulty)->render();

        m_texture_manager.get(MainMenuTextureManager::SETTINGS_BUTTON)->render();
    }
};

Game::Difficulty MainMenuScreen::selected_difficulty = Game::DIFFIC_LOWEST;
