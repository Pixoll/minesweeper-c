#pragma once

#include <memory>
#include <SDL.h>

#include "../graphics/font.hpp"
#include "../graphics/texture.hpp"

class SettingsTextureManager {
public:
    enum TextureName {
        BACK_BUTTON,
    };

    using SettingsTexture = std::shared_ptr<Texture>;

private:
    static constexpr auto BACK_BUTTON_IMAGE_PATH = "assets/textures/button_back.png";

    SDL_Renderer *m_renderer;
    const int m_window_width;
    const int m_window_height;
    const int m_window_padding;

    SettingsTexture m_back_button_texture;

public:
    SettingsTextureManager(SDL_Renderer *renderer, const int window_width, const int window_height) :
        m_renderer(renderer),
        m_window_width(window_width),
        m_window_height(window_height),
        m_window_padding(window_height * 0.025) {
        make_back_button_texture();
    }

    ~SettingsTextureManager() = default;

    [[nodiscard]] SettingsTexture get(const TextureName name) const {
        switch (name) {
            case BACK_BUTTON: return m_back_button_texture;
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
};
