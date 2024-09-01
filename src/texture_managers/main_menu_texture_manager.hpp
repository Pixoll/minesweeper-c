#pragma once

#include <memory>
#include <SDL.h>

#include "../core/game.hpp"
#include "../graphics/color.hpp"
#include "../graphics/font.hpp"
#include "../graphics/shape.hpp"
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
    static constexpr auto SETTINGS_BUTTON_IMAGE_PATH = "assets/textures/button_settings.png";

    // height:width
    static constexpr double GAME_BUTTON_RATIO = 41.0 / 300;
    // height:width
    static constexpr double ARROW_RATIO = 25.0 / 14;
    static constexpr double GAME_BUTTON_THICKNESS_FACTOR = 3.0 / 91;
    static constexpr double ARROW_THICKNESS_FACTOR = 1.0 / 4;
    static constexpr double QUIT_BUTTON_THICKNESS_FACTOR = 1.0 / 7;

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
        make_big_mine_texture();
        make_title_texture();
        make_quit_button();
        make_bottom_buttons();
        make_new_game_button();
        make_continue_game_button();
        make_difficulty_buttons();
        make_difficulty_textures();
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
    void make_big_mine_texture() {
        const int y = m_window_height * 0.1;
        const int size = m_window_height * 0.25;
        const int x = (m_window_width - size) / 2;

        m_big_mine_texture = std::make_shared<Texture>(
            m_renderer,
            BIG_MINE_IMAGE_PATH,
            SDL_Rect{x, y, size, size}
        );

        m_big_mine_texture->set_color(Color::THEME);
    }

    void make_title_texture() {
        const Font title_font(Font::RUBIK_LIGHT, m_window_height * 0.04);

        m_title_texture = std::make_shared<Texture>(m_renderer, title_font.get_raw(), "Minesweeper", Color::WHITE);
        m_title_texture->set_position(
            (m_window_width - m_title_texture->get_w()) / 2,
            static_cast<int>((m_big_mine_texture->get_y() + m_big_mine_texture->get_h()) * 1.1)
        );
    }

    void make_quit_button() {
        const int size = Font::get_shared(Font::PRIMARY)->get_size();
        const int x = m_window_width - size - m_window_padding;
        const int y = m_window_padding;
        const float thickness = size * QUIT_BUTTON_THICKNESS_FACTOR;
        const float end = size - thickness;

        m_quit_button_texture = std::make_shared<Texture>(m_renderer, SDL_Rect{x, y, size, size});

        const Texture::ScopedRender scoped_render = m_quit_button_texture->set_as_render_target();

        Shape::rounded_line(m_renderer, thickness, thickness, end, end, thickness, Color::WHITE);
        Shape::rounded_line(m_renderer, thickness, end, end, thickness, thickness, Color::WHITE);
    }

    void make_new_game_button() {
        const int width = m_big_mine_texture->get_w() * 1.86f;
        const int height = width * GAME_BUTTON_RATIO;
        const int thickness = height * GAME_BUTTON_THICKNESS_FACTOR;
        const int x = m_big_mine_texture->get_x() + (m_big_mine_texture->get_w() - width) / 2;

        Texture text_texture(m_renderer, Font::get_shared(Font::PRIMARY)->get_raw(), "New game", Color::WHITE);
        text_texture.set_position(
            (width - text_texture.get_w()) / 2,
            (height - text_texture.get_h()) / 2
        );

        m_new_game_button_texture = std::make_shared<Texture>(
            m_renderer,
            SDL_Rect{
                x,
                m_window_height / 2 + static_cast<int>(height * 1.5),
                width,
                height,
            }
        );

        const Texture::ScopedRender scoped_render = m_new_game_button_texture->set_as_render_target();

        Shape::rounded_rectangle(
            m_renderer,
            {0, 0, width, height},
            thickness,
            height / 2.0f,
            Color::LIGHTER_GREY
        );

        text_texture.render();
    }

    void make_continue_game_button() {
        const int width = m_new_game_button_texture->get_w();
        const int height = width * GAME_BUTTON_RATIO;
        const int thickness = height * GAME_BUTTON_THICKNESS_FACTOR;

        Texture text_texture(m_renderer, Font::get_shared(Font::PRIMARY)->get_raw(), "Continue", Color::WHITE);
        text_texture.set_position(
            (width - text_texture.get_w()) / 2,
            (height - text_texture.get_h()) / 2
        );

        m_continue_game_button_texture = std::make_shared<Texture>(
            m_renderer,
            SDL_Rect{
                m_new_game_button_texture->get_x(),
                static_cast<int>(m_new_game_button_texture->get_y() + m_new_game_button_texture->get_h() * 1.5),
                width,
                height,
            }
        );

        const Texture::ScopedRender scoped_render = m_continue_game_button_texture->set_as_render_target();

        Shape::rounded_rectangle(
            m_renderer,
            {0, 0, width, height},
            thickness,
            height / 2.0f,
            Color::LIGHTER_GREY
        );

        text_texture.render();
    }

    void make_difficulty_buttons() {
        const Font::Shared font = Font::get_shared(Font::PRIMARY);
        const int button_width = m_new_game_button_texture->get_w();
        const int offset = button_width * 0.09;
        const int height = font->get_size();
        const double width = height / ARROW_RATIO;
        const double thickness = width * ARROW_THICKNESS_FACTOR;
        const int y = m_new_game_button_texture->get_y() - m_new_game_button_texture->get_h() - height / 2;

        m_left_arrow_texture = std::make_shared<Texture>(
            m_renderer,
            SDL_Rect{
                m_new_game_button_texture->get_x() + offset,
                y,
                static_cast<int>(width),
                height
            }
        );

        const Texture::ScopedRender left_arrow_renderer = m_left_arrow_texture->set_as_render_target();

        Shape::rounded_line(
            m_renderer,
            thickness,
            (height + thickness) / 2.0,
            width - thickness,
            height - thickness,
            thickness,
            Color::WHITE
        );
        Shape::rounded_line(
            m_renderer,
            thickness,
            (height - thickness) / 2.0,
            width - thickness,
            thickness,
            thickness,
            Color::WHITE
        );

        left_arrow_renderer.release();

        m_right_arrow_texture = std::make_shared<Texture>(
            m_renderer,
            SDL_Rect{
                m_new_game_button_texture->get_x() + button_width - offset - m_left_arrow_texture->get_w(),
                y,
                static_cast<int>(width),
                height
            }
        );

        const Texture::ScopedRender right_arrow_renderer = m_right_arrow_texture->set_as_render_target();

        Shape::rounded_line(
            m_renderer,
            thickness,
            height - thickness,
            width - thickness,
            (height + thickness) / 2.0,
            thickness,
            Color::WHITE
        );
        Shape::rounded_line(
            m_renderer,
            thickness,
            thickness,
            width - thickness,
            (height - thickness) / 2.0,
            thickness,
            Color::WHITE
        );
    }

    void make_difficulty_textures() {
        const Font::Shared font = Font::get_shared(Font::PRIMARY);
        const int button_width = m_new_game_button_texture->get_w();
        const int x_offset = m_new_game_button_texture->get_x();
        const int y = m_new_game_button_texture->get_y() - m_new_game_button_texture->get_h() - font->get_size() / 2;

        for (int i = 0; i < Game::DIFFICULTIES; i++) {
            MainMenuTexture &difficulty_texture = m_difficulty_textures[i];
            difficulty_texture = std::make_shared<Texture>(
                m_renderer,
                font->get_raw(),
                Game::DIFFICULTY_NAMES[i],
                Color::WHITE
            );
            difficulty_texture->set_position(x_offset + (button_width - difficulty_texture->get_w()) / 2, y);
        }
    }

    void make_bottom_buttons() {
        const int height = Font::get_shared(Font::PRIMARY)->get_size() * 1.5;

        m_settings_button_texture = std::make_shared<Texture>(m_renderer, SETTINGS_BUTTON_IMAGE_PATH);
        m_settings_button_texture->set_height(height);
        m_settings_button_texture->set_position(
            (m_window_width - m_settings_button_texture->get_w()) / 2,
            m_window_height - height - m_window_padding
        );
    }
};
