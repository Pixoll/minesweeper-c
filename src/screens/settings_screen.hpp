#pragma once

#include <SDL.h>

#include "screen.hpp"
#include "../texture_managers/settings_texture_manager.hpp"

class Engine;
class MainMenuScreen;

class SettingsScreen final : virtual public Screen {
    using SettingsTexture = SettingsTextureManager::SettingsTexture;
    using SettingsTextureBundle = SettingsTextureManager::SettingsTextureBundle;
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

        render_setting(TextureBundleName::SETTING_SHOW_CELL_BORDERS, Settings::SHOW_CELL_BORDERS);
        render_setting(TextureBundleName::SETTING_SHOW_CONTROLS, Settings::SHOW_CONTROLS);
        render_setting(TextureBundleName::SETTING_SWAP_CONTROLS, Settings::SWAP_CONTROLS);
        render_setting(TextureBundleName::SETTING_EASY_DIG, Settings::EASY_DIG);
        render_setting(TextureBundleName::SETTING_EASY_FLAG, Settings::EASY_FLAG);
    }

    void render_setting(const TextureBundleName bundle_name, const Settings::Name setting_name) const {
        const SettingsTextureBundle texture_bundle = m_texture_manager.get(bundle_name);

        if (texture_bundle->get_y() > m_window_height)
            return;

        texture_bundle->render();

        const SettingsTexture toggle_on_texture = m_texture_manager.get(TextureName::TOGGLE_ON);
        const SettingsTexture toggle_off_texture = m_texture_manager.get(TextureName::TOGGLE_OFF);
        const int toggle_y = Font::get_shared(Font::SECONDARY)->get_size()
                + texture_bundle->get_y()
                + texture_bundle->get_h();

        if (toggle_y > m_window_height)
            return;

        if (Settings::is_on(setting_name))
            toggle_on_texture->render_to(toggle_on_texture->get_x(), toggle_y);
        else
            toggle_off_texture->render_to(toggle_off_texture->get_x(), toggle_y);
    }
};
