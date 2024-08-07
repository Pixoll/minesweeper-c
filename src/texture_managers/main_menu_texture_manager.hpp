#pragma once

#include <memory>
#include <SDL.h>

#include "../graphics/color.hpp"
#include "../graphics/font.hpp"
#include "../graphics/texture.hpp"

class MainMenuTextureManager {
public:
    enum Name {
        BIG_MINE,
        TITLE,
        NEW_GAME_BUTTON,
        CONTINUE_GAME_BUTTON,
    };

    using MainMenuTexture = std::shared_ptr<Texture>;

private:
    static constexpr auto BIG_MINE_IMAGE_PATH = "assets/textures/mine_big.png";
    static constexpr auto GAME_BUTTON_IMAGE_PATH = "assets/textures/button_game.png";

    SDL_Renderer *m_renderer;
    const int m_window_width;
    const int m_window_height;

    MainMenuTexture m_big_mine_texture;
    MainMenuTexture m_title_texture;

    MainMenuTexture m_new_game_button_texture;
    MainMenuTexture m_continue_game_button_texture;

    Font m_title_font;

public:
    MainMenuTextureManager(SDL_Renderer *renderer, const int window_width, const int window_height) :
        m_renderer(renderer),
        m_window_width(window_width),
        m_window_height(window_height),
        m_title_font(Font::RUBIK_LIGHT, m_window_height * 0.04) {
        init_big_mine_texture();
        init_title_texture();

        const int width = m_big_mine_texture->get_w() * 1.86f;
        const int x = m_big_mine_texture->get_x() + (m_big_mine_texture->get_w() - width) / 2;
        init_new_game_button(width, x);
        init_continute_game_button(width, x);
    }

    ~MainMenuTextureManager() = default;

    [[nodiscard]] MainMenuTexture get(const Name name) const {
        switch (name) {
            case BIG_MINE: return m_big_mine_texture;
            case TITLE: return m_title_texture;
            case NEW_GAME_BUTTON: return m_new_game_button_texture;
            case CONTINUE_GAME_BUTTON: return m_continue_game_button_texture;
        }
        __builtin_unreachable();
    }

private:
    void init_big_mine_texture() {
        const int y = m_window_height * 0.1;
        const int size = m_window_height * 0.25;
        const int x = (m_window_width - size) / 2;

        m_big_mine_texture = std::make_shared<Texture>(
            m_renderer,
            BIG_MINE_IMAGE_PATH,
            SDL_Rect{x, y, size, size}
        );

        m_big_mine_texture->set_color_mod(Color::THEME);
    }

    void init_title_texture() {
        m_title_texture = std::make_shared<Texture>(m_renderer, m_title_font.get_font(), "Minesweeper", Color::WHITE);
        m_title_texture->set_position(
            (m_window_width - m_title_texture->get_w()) / 2,
            static_cast<int>((m_big_mine_texture->get_y() + m_big_mine_texture->get_h()) * 1.1)
        );
    }

    void init_new_game_button(const int width, const int x) {
        Texture button_texture(m_renderer, GAME_BUTTON_IMAGE_PATH);
        button_texture.set_color_mod(Color::LIGHTER_GREY);
        button_texture.set_width(width);

        m_new_game_button_texture = std::make_shared<Texture>(
            m_renderer,
            SDL_Rect{
                x,
                m_window_height / 2 + static_cast<int>(button_texture.get_h() * 1.5),
                width,
                button_texture.get_h(),
            }
        );
        const Texture::ScopedRender scoped_render = m_new_game_button_texture->set_as_render_target();

        button_texture.render();

        Texture text_texture(m_renderer, Font::get_shared(Font::PRIMARY)->get_font(), "New game", Color::WHITE);
        text_texture.set_position(
            (button_texture.get_w() - text_texture.get_w()) / 2,
            (button_texture.get_h() - text_texture.get_h()) / 2
        );
        text_texture.render();
    }

    void init_continute_game_button(const int width, const int x) {
        Texture button_texture(m_renderer, GAME_BUTTON_IMAGE_PATH);
        button_texture.set_color_mod(Color::LIGHTER_GREY);
        button_texture.set_width(width);

        m_continue_game_button_texture = std::make_shared<Texture>(
            m_renderer,
            SDL_Rect{
                x,
                static_cast<int>(m_new_game_button_texture->get_y() + m_new_game_button_texture->get_h() * 1.5),
                width,
                button_texture.get_h(),
            }
        );
        const Texture::ScopedRender scoped_render = m_continue_game_button_texture->set_as_render_target();

        button_texture.render();

        Texture text_texture(m_renderer, Font::get_shared(Font::PRIMARY)->get_font(), "Continue", Color::WHITE);
        text_texture.set_position(
            (button_texture.get_w() - text_texture.get_w()) / 2,
            (button_texture.get_h() - text_texture.get_h()) / 2
        );
        text_texture.render();
    }
};
