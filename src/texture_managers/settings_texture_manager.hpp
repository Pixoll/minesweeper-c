#pragma once

#include <memory>
#include <SDL.h>

#include "../graphics/color.hpp"
#include "../graphics/font.hpp"
#include "../graphics/texture.hpp"
#include "../graphics/texture_bundle.hpp"

class SettingsTextureManager {
public:
    enum TextureName {
        BACK_BUTTON,
        TOGGLE_OFF,
        TOGGLE_ON,
    };

    enum TextureBundleName {
        SETTING_SHOW_CELL_BORDERS,
        SETTING_SHOW_CONTROLS,
        SETTING_SWAP_CONTROLS,
        SETTING_EASY_DIG,
        SETTING_EASY_FLAG,
    };

    using SettingsTexture = std::shared_ptr<Texture>;
    using SettingsTextureBundle = std::shared_ptr<TextureBundle>;

private:
    static constexpr auto BACK_BUTTON_IMAGE_PATH = "assets/textures/button_back.png";
    static constexpr auto TOGGLE_OFF_IMAGE_PATH = "assets/textures/toggle_off.png";
    static constexpr auto TOGGLE_ON_IMAGE_PATH = "assets/textures/toggle_on.png";

    SDL_Renderer *m_renderer;
    const int m_window_width;
    const int m_window_height;
    const int m_window_padding;

    SettingsTexture m_back_button_texture;

    SettingsTexture m_toggle_off_texture;
    SettingsTexture m_toggle_on_texture;

    SettingsTextureBundle m_show_cell_borders_text_texture_bundle = std::make_shared<TextureBundle>();
    SettingsTextureBundle m_show_controls_text_texture_bundle = std::make_shared<TextureBundle>();
    SettingsTextureBundle m_swap_controls_text_texture_bundle = std::make_shared<TextureBundle>();
    SettingsTextureBundle m_easy_dig_text_texture_bundle = std::make_shared<TextureBundle>();
    SettingsTextureBundle m_easy_flag_text_texture_bundle = std::make_shared<TextureBundle>();

public:
    SettingsTextureManager(SDL_Renderer *renderer, const int window_width, const int window_height) :
        m_renderer(renderer),
        m_window_width(window_width),
        m_window_height(window_height),
        m_window_padding(window_height * 0.025) {
        make_back_button_texture();
        make_toggles_textures();

        make_setting_text_texture_bundle(
            m_show_cell_borders_text_texture_bundle,
            "Show cell borders",
            "Choose whether to display cell borders or not."
        );

        make_setting_text_texture_bundle(
            m_show_controls_text_texture_bundle,
            "Show controls",
            "Choose whether to display the controls in the corner of the game or not."
        );

        make_setting_text_texture_bundle(
            m_swap_controls_text_texture_bundle,
            "Swap controls",
            "Choose whether to swap the game controls or not."
        );

        make_setting_text_texture_bundle(
            m_easy_dig_text_texture_bundle,
            "Easy digging",
            "Clicking a number will dig all of its surrounding unflagged cells with one click.\n"
            "It will work if the amount of surrounding flagged cells matches the clicked digit."
        );

        make_setting_text_texture_bundle(
            m_easy_flag_text_texture_bundle,
            "Easy flagging",
            "Clicking a number will flag all of its surrounding covered cells with one click.\n"
            "It will work if the amount of surrounding closed covered matches the clicked digit."
        );
    }

    ~SettingsTextureManager() = default;

    [[nodiscard]] SettingsTexture get(const TextureName name) const {
        switch (name) {
            case BACK_BUTTON: return m_back_button_texture;
            case TOGGLE_OFF: return m_toggle_off_texture;
            case TOGGLE_ON: return m_toggle_on_texture;
        }
        __builtin_unreachable();
    }

    [[nodiscard]] SettingsTextureBundle get(const TextureBundleName bundle_name) const {
        switch (bundle_name) {
            case SETTING_SHOW_CELL_BORDERS: return m_show_cell_borders_text_texture_bundle;
            case SETTING_SHOW_CONTROLS: return m_show_controls_text_texture_bundle;
            case SETTING_SWAP_CONTROLS: return m_swap_controls_text_texture_bundle;
            case SETTING_EASY_DIG: return m_easy_dig_text_texture_bundle;
            case SETTING_EASY_FLAG: return m_easy_flag_text_texture_bundle;
        }
        __builtin_unreachable();
    }

private:
    void make_back_button_texture() {
        const int height = Font::get_shared(Font::PRIMARY)->get_size();
        m_back_button_texture = std::make_shared<Texture>(m_renderer, BACK_BUTTON_IMAGE_PATH);
        m_back_button_texture->set_position(m_window_padding, m_window_padding);
        m_back_button_texture->set_height(height);
    }

    void make_toggles_textures() {
        const int toggle_size = m_window_height * 0.17;
        const Font font(Font::RUBIK_LIGHT, m_window_height * 0.03);

        m_toggle_off_texture = std::make_shared<Texture>(
            m_renderer,
            TOGGLE_OFF_IMAGE_PATH,
            SDL_Rect{0, 0, toggle_size, toggle_size}
        );
        m_toggle_off_texture->set_color_mod(Color::LIGHT_GREY);

        const Texture::ScopedRender texture_off_scoped_render = m_toggle_off_texture->set_as_render_target();
        const Texture text_off_texture(m_renderer, font.get_font(), "OFF", Color::WHITE);
        text_off_texture.render();
        texture_off_scoped_render.release();

        m_toggle_on_texture = std::make_shared<Texture>(
            m_renderer,
            TOGGLE_ON_IMAGE_PATH,
            SDL_Rect{0, 0, toggle_size, toggle_size}
        );
        m_toggle_on_texture->set_color_mod(Color::THEME);

        const Texture::ScopedRender texture_on_scoped_render = m_toggle_off_texture->set_as_render_target();
        const Texture text_on_texture(m_renderer, font.get_font(), "ON", Color::BACKGROUND);
        text_on_texture.render();
        texture_on_scoped_render.release();
    }

    void make_setting_text_texture_bundle(
        const SettingsTextureBundle &texture_bundle,
        const char *name,
        const char *description
    ) const {
        const Font::Shared primary_font = Font::get_shared(Font::PRIMARY);

        texture_bundle->add(
            {
                m_renderer,
                primary_font->get_font(),
                name,
                Color::WHITE
            }
        );

        texture_bundle->add(
            {
                m_renderer,
                Font::get_shared(Font::SECONDARY)->get_font(),
                description,
                Color::LIGHTER_GREY,
                {0, texture_bundle->operator[](0)->get_h() + primary_font->get_size()}
            }
        );
    }
};
