#pragma once

#include <SDL.h>

#include "screen.hpp"
#include "../texture_managers/settings_texture_manager.hpp"

class Engine;
class MainMenuScreen;

class SettingsScreen final : virtual public Screen {
    using TextureName = SettingsTextureManager::TextureName;
    using TextureBundleName = SettingsTextureManager::TextureBundleName;

    Engine *m_engine;
    int m_window_width;
    int m_window_height;
    SettingsTextureManager m_texture_manager;

    SDL_Cursor *const m_arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    SDL_Cursor *const m_hand_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

public:
    explicit SettingsScreen(Engine *engine) :
        m_engine(engine),
        m_window_width(engine->get_window_width()),
        m_window_height(engine->get_window_height()),
        m_texture_manager(engine->get_renderer(), m_window_width, m_window_height) {}

    ~SettingsScreen() override = default;

    void run_logic(const SDL_Event &event) override {
        SDL_Point cursor_pos;
        SDL_GetMouseState(&cursor_pos.x, &cursor_pos.y);

        const bool cursor_in_back_button = m_texture_manager.get(TextureName::BACK_BUTTON)->contains(cursor_pos);

        SDL_SetCursor(cursor_in_back_button ? m_hand_cursor : m_arrow_cursor);

        if (event.type != SDL_MOUSEBUTTONDOWN || event.button.button != SDL_BUTTON_LEFT)
            return;

        if (cursor_in_back_button) {
            m_engine->set_screen<MainMenuScreen>(m_engine);
            return;
        }
    }

    void render() override {
        m_texture_manager.get(TextureName::BACK_BUTTON)->render();
        m_texture_manager.get(TextureName::TOGGLE_OFF)->render();

        m_texture_manager.get(TextureBundleName::SETTING_SHOW_CELL_BORDERS)->render();
        m_texture_manager.get(TextureBundleName::SETTING_SHOW_CONTROLS)->render();
        m_texture_manager.get(TextureBundleName::SETTING_SWAP_CONTROLS)->render();
        m_texture_manager.get(TextureBundleName::SETTING_EASY_DIG)->render();
        m_texture_manager.get(TextureBundleName::SETTING_EASY_FLAG)->render();
    }
};
