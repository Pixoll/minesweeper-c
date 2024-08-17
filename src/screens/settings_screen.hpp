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

    static constexpr int SETTINGS_AMOUNT = Settings::EASY_FLAG + 1;

    Engine *m_engine;
    int m_window_width;
    int m_window_height;
    SettingsTextureManager m_texture_manager;
    int m_scroll_step;
    int m_max_scroll;
    int m_scrollbar_max_y;
    int m_scrollbar_step;
    int m_settings_delta_y = 0;
    int m_scrollbar_y = 0;

    SDL_Cursor *const m_arrow_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    SDL_Cursor *const m_hand_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

public:
    explicit SettingsScreen(Engine *engine) :
        m_engine(engine),
        m_window_width(engine->get_window_width()),
        m_window_height(engine->get_window_height()),
        m_texture_manager(engine->get_renderer(), m_window_width, m_window_height),
        m_scroll_step(m_window_width * 0.03),
        m_max_scroll(m_window_height / 2 - m_texture_manager.get_settings_total_height()),
        m_scrollbar_max_y(m_window_height - m_texture_manager.get(TextureName::SCROLLBAR)->get_h()),
        m_scrollbar_step(m_scrollbar_max_y / (-m_max_scroll / m_scroll_step)) {}

    ~SettingsScreen() override = default;

    void run_logic(const SDL_Event &event) override {
        SDL_Point cursor_pos;
        SDL_GetMouseState(&cursor_pos.x, &cursor_pos.y);

        Settings::Name selected_setting;

        const bool cursor_in_back_button = m_texture_manager.get(TextureName::BACK_BUTTON)->contains(cursor_pos);
        const bool cursor_in_scrollbar = m_texture_manager.get(TextureName::SCROLLBAR)->contains(cursor_pos);
        const bool cursor_in_setting_toggle = mouse_on_setting_toggle(cursor_pos, &selected_setting);

        SDL_SetCursor(
            cursor_in_back_button || cursor_in_scrollbar || cursor_in_setting_toggle
            ? m_hand_cursor
            : m_arrow_cursor
        );

        if (event.type == SDL_MOUSEWHEEL) {
            const float dy = event.wheel.preciseY;

            m_settings_delta_y += dy * m_scroll_step;
            m_scrollbar_y -= dy * m_scrollbar_step;

            if (m_settings_delta_y > 0)
                m_settings_delta_y = 0;

            if (m_settings_delta_y < m_max_scroll)
                m_settings_delta_y = m_max_scroll;

            if (m_scrollbar_y < 0)
                m_scrollbar_y = 0;

            if (m_scrollbar_y > m_scrollbar_max_y)
                m_scrollbar_y = m_scrollbar_max_y;

            return;
        }

        if (event.type != SDL_MOUSEBUTTONDOWN || event.button.button != SDL_BUTTON_LEFT)
            return;

        if (cursor_in_back_button) {
            Settings::save();
            m_engine->set_screen<MainMenuScreen>(m_engine);
            return;
        }

        if (cursor_in_setting_toggle) {
            Settings::toggle(selected_setting);
            return;
        }
    }

    void render() override {
        m_texture_manager.get(TextureName::BACK_BUTTON)->render();
        m_texture_manager.get(TextureName::SCROLLBAR)->render_moved(0, m_scrollbar_y);

        for (int bundle_name = 0; bundle_name < SETTINGS_AMOUNT; ++bundle_name)
            render_setting(static_cast<TextureBundleName>(bundle_name));
    }

private:
    void render_setting(const TextureBundleName bundle_name) const {
        const SettingsTextureBundle texture_bundle = m_texture_manager.get(bundle_name);
        const SettingsTexture toggle_on_texture = m_texture_manager.get(TextureName::TOGGLE_ON);
        const SettingsTexture toggle_off_texture = m_texture_manager.get(TextureName::TOGGLE_OFF);
        const int toggle_y = Font::get_shared(Font::SECONDARY)->get_size()
                + texture_bundle->get_y()
                + texture_bundle->get_h()
                + m_settings_delta_y;

        if (texture_bundle->get_y() + m_settings_delta_y > m_window_height || toggle_y + toggle_on_texture->get_h() < 0)
            return;

        texture_bundle->render_moved(0, m_settings_delta_y);

        if (toggle_y > m_window_height)
            return;

        if (Settings::is_on(static_cast<Settings::Name>(bundle_name)))
            toggle_on_texture->render_to(toggle_on_texture->get_x(), toggle_y);
        else
            toggle_off_texture->render_to(toggle_off_texture->get_x(), toggle_y);
    }

    [[nodiscard]] bool mouse_on_setting_toggle(const SDL_Point &cursor_pos, Settings::Name *hovered_setting) const {
        // TODO is std::any_of better? (clang suggestion)
        for (int bundle_name = 0; bundle_name < SETTINGS_AMOUNT; ++bundle_name) {
            const SettingsTextureBundle texture_bundle = m_texture_manager.get(
                static_cast<TextureBundleName>(bundle_name)
            );
            const SettingsTexture toggle_texture = m_texture_manager.get(TextureName::TOGGLE_ON);

            const int toggle_y = Font::get_shared(Font::SECONDARY)->get_size()
                    + texture_bundle->get_y()
                    + texture_bundle->get_h()
                    + m_settings_delta_y;

            if (toggle_texture->contains_moved(0, toggle_y, cursor_pos)) {
                *hovered_setting = static_cast<Settings::Name>(bundle_name);
                return true;
            }
        }

        return false;
    }
};
