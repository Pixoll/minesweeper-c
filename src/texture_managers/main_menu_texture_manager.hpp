#pragma once

#include <memory>
#include <SDL.h>

#include "../game.hpp"
#include "../graphics/color.hpp"
#include "../graphics/font.hpp"
#include "../graphics/texture.hpp"

class MainMenuTextureManager {
public:
    enum TextureName {
        BIG_MINE,
        TITLE,
        NEW_GAME_BUTTON,
        CONTINUE_GAME_BUTTON,
        LEFT_ARROW,
        RIGHT_ARROW,
        SETTINGS_BUTTON,
        QUIT_BUTTON,
    };

    using MainMenuTexture = std::shared_ptr<Texture>;

private:
    static constexpr auto BIG_MINE_IMAGE_PATH = "assets/textures/mine_big.png";
    static constexpr auto GAME_BUTTON_IMAGE_PATH = "assets/textures/button_game.png";
    static constexpr auto LEFT_ARROW_IMAGE_PATH = "assets/textures/arrow_left.png";
    static constexpr auto RIGHT_ARROW_IMAGE_PATH = "assets/textures/arrow_right.png";
    static constexpr auto QUIT_BUTTON_IMAGE_PATH = "assets/textures/button_quit.png";
    static constexpr auto SETTINGS_BUTTON_IMAGE_PATH = "assets/textures/button_settings.png";

    SDL_Renderer *m_renderer;
    const int m_window_width;
    const int m_window_height;
    const int m_window_padding;

    MainMenuTexture m_big_mine_texture;
    MainMenuTexture m_title_texture;

    MainMenuTexture m_new_game_button_texture;
    MainMenuTexture m_continue_game_button_texture;

    MainMenuTexture m_left_arrow_texture;
    MainMenuTexture m_right_arrow_texture;
    MainMenuTexture m_difficulty_textures[Game::DIFFICULTIES];

    MainMenuTexture m_quit_button_texture;
    MainMenuTexture m_settings_button_texture;

public:
    MainMenuTextureManager(SDL_Renderer *renderer, const int window_width, const int window_height) :
        m_renderer(renderer),
        m_window_width(window_width),
        m_window_height(window_height),
        m_window_padding(window_height * 0.025) {
        init_big_mine_texture();
        init_title_texture();
        init_quit_button();
        init_bottom_buttons();
        init_new_game_button();
        init_continute_game_button();
        init_difficulty_buttons();
        init_difficulty_textures();
    }

    ~MainMenuTextureManager() = default;

    [[nodiscard]] MainMenuTexture get(const TextureName name) const {
        switch (name) {
            case BIG_MINE: return m_big_mine_texture;
            case TITLE: return m_title_texture;
            case NEW_GAME_BUTTON: return m_new_game_button_texture;
            case CONTINUE_GAME_BUTTON: return m_continue_game_button_texture;
            case LEFT_ARROW: return m_left_arrow_texture;
            case RIGHT_ARROW: return m_right_arrow_texture;
            case SETTINGS_BUTTON: return m_settings_button_texture;
            case QUIT_BUTTON: return m_quit_button_texture;
        }
        __builtin_unreachable();
    }

    [[nodiscard]] MainMenuTexture get(const Game::Difficulty difficulty) const {
        return m_difficulty_textures[difficulty];
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
        const Font title_font(Font::RUBIK_LIGHT, m_window_height * 0.04);

        m_title_texture = std::make_shared<Texture>(m_renderer, title_font.get_font(), "Minesweeper", Color::WHITE);
        m_title_texture->set_position(
            (m_window_width - m_title_texture->get_w()) / 2,
            static_cast<int>((m_big_mine_texture->get_y() + m_big_mine_texture->get_h()) * 1.1)
        );
    }

    void init_quit_button() {
        const int size = Font::get_shared(Font::PRIMARY)->get_size();
        const int x = m_window_width - size - m_window_padding;
        m_quit_button_texture = std::make_shared<Texture>(
            m_renderer,
            QUIT_BUTTON_IMAGE_PATH,
            SDL_Rect{x, m_window_padding, size, size}
        );
    }

    void init_new_game_button() {
        const int width = m_big_mine_texture->get_w() * 1.86f;
        const int x = m_big_mine_texture->get_x() + (m_big_mine_texture->get_w() - width) / 2;

        Texture button_texture(m_renderer, GAME_BUTTON_IMAGE_PATH);
        button_texture.set_color_mod(Color::LIGHTER_GREY);
        button_texture.set_width(width);

        Texture text_texture(m_renderer, Font::get_shared(Font::PRIMARY)->get_font(), "New game", Color::WHITE);
        text_texture.set_position(
            (button_texture.get_w() - text_texture.get_w()) / 2,
            (button_texture.get_h() - text_texture.get_h()) / 2
        );

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
        text_texture.render();
    }

    void init_continute_game_button() {
        const int width = m_new_game_button_texture->get_w();

        Texture button_texture(m_renderer, GAME_BUTTON_IMAGE_PATH);
        button_texture.set_color_mod(Color::LIGHTER_GREY);
        button_texture.set_width(width);

        Texture text_texture(m_renderer, Font::get_shared(Font::PRIMARY)->get_font(), "Continue", Color::WHITE);
        text_texture.set_position(
            (button_texture.get_w() - text_texture.get_w()) / 2,
            (button_texture.get_h() - text_texture.get_h()) / 2
        );

        m_continue_game_button_texture = std::make_shared<Texture>(
            m_renderer,
            SDL_Rect{
                m_new_game_button_texture->get_x(),
                static_cast<int>(m_new_game_button_texture->get_y() + m_new_game_button_texture->get_h() * 1.5),
                width,
                button_texture.get_h(),
            }
        );

        const Texture::ScopedRender scoped_render = m_continue_game_button_texture->set_as_render_target();

        button_texture.render();
        text_texture.render();
    }

    void init_difficulty_buttons() {
        const Font::Shared font = Font::get_shared(Font::PRIMARY);
        const int button_width = m_new_game_button_texture->get_w();
        const int arrow_offset = button_width * 0.09;
        const int h = font->get_size();
        const int y = m_new_game_button_texture->get_y() - m_new_game_button_texture->get_h() - h / 2;

        m_left_arrow_texture = std::make_shared<Texture>(m_renderer, LEFT_ARROW_IMAGE_PATH);
        m_left_arrow_texture->set_position(m_new_game_button_texture->get_x() + arrow_offset, y);
        m_left_arrow_texture->set_height(h);

        m_right_arrow_texture = std::make_shared<Texture>(m_renderer, RIGHT_ARROW_IMAGE_PATH);
        m_right_arrow_texture->set_position(
            m_new_game_button_texture->get_x() + button_width - arrow_offset - m_left_arrow_texture->get_w(),
            y
        );
        m_right_arrow_texture->set_height(h);
    }

    void init_difficulty_textures() {
        const Font::Shared font = Font::get_shared(Font::PRIMARY);
        const int button_width = m_new_game_button_texture->get_w();
        const int x_offset = m_new_game_button_texture->get_x();
        const int y = m_new_game_button_texture->get_y() - m_new_game_button_texture->get_h() - font->get_size() / 2;

        for (int i = 0; i < Game::DIFFICULTIES; i++) {
            MainMenuTexture &difficulty_texture = m_difficulty_textures[i];
            difficulty_texture = std::make_shared<Texture>(
                m_renderer,
                font->get_font(),
                Game::DIFFICULTY_NAMES[i],
                Color::WHITE
            );
            difficulty_texture->set_position(x_offset + (button_width - difficulty_texture->get_w()) / 2, y);
        }
    }

    void init_bottom_buttons() {
        const int height = Font::get_shared(Font::PRIMARY)->get_size() * 1.5;

        m_settings_button_texture = std::make_shared<Texture>(m_renderer, SETTINGS_BUTTON_IMAGE_PATH);
        m_settings_button_texture->set_height(height);
        m_settings_button_texture->set_position(
            (m_window_width - m_settings_button_texture->get_w()) / 2,
            m_window_height - height - m_window_padding
        );
    }
};
