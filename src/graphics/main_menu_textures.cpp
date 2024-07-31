#include "main_menu_textures.hpp"

#include <SDL.h>

#include "colors.hpp"
#include "fonts.hpp"
#include "texture.hpp"

Texture big_mine_texture;
Texture title_texture;
Texture new_game_button_texture;
Texture continue_game_button_texture;

const auto big_mine_path = "assets/textures/mine_big.png";
const auto game_button_path = "assets/textures/button_game.png";

void init_big_mine_texture(SDL_Renderer *renderer, const int window_width, const int window_height) {
    const int y = window_height * 0.1;
    const int size = window_height * 0.25;
    const int x = (window_width - size) / 2;

    big_mine_texture = {
        renderer,
        big_mine_path,
        {x, y, size, size}
    };

    big_mine_texture.set_color_mod(Color::THEME);
}

void init_title_texture(SDL_Renderer *renderer, const int window_width) {
    title_texture = {renderer, Font::TITLE, "Minesweeper", Color::WHITE};
    title_texture.set_position(
        (window_width - title_texture.get_w()) / 2,
        static_cast<int>((big_mine_texture.get_y() + big_mine_texture.get_h()) * 1.1)
    );
}

void init_game_buttons(SDL_Renderer *renderer, const int window_height) {
    const SDL_Color lighter_grey = get_color(Color::LIGHTER_GREY).rgb;
    const int width = big_mine_texture.get_w() * 1.86f;
    const int x = big_mine_texture.get_x() + (big_mine_texture.get_w() - width) / 2;

    {
        Texture button_texture(renderer, game_button_path);
        button_texture.set_color_mod(lighter_grey);
        button_texture.set_width(width);

        new_game_button_texture = {
            renderer,
            {
                x,
                window_height / 2 + static_cast<int>(button_texture.get_h() * 1.5),
                width,
                button_texture.get_h(),
            },
        };
        const Texture::ScopedRender scoped_render = new_game_button_texture.set_as_render_target();

        button_texture.render();

        Texture text_texture(renderer, Font::PRIMARY, "New game", Color::WHITE);
        text_texture.set_position(
            (button_texture.get_w() - text_texture.get_w()) / 2,
            (button_texture.get_h() - text_texture.get_h()) / 2
        );
        text_texture.render();

        button_texture.destroy();
        text_texture.destroy();
    }

    {
        Texture button_texture(renderer, game_button_path);
        button_texture.set_color_mod(lighter_grey);
        button_texture.set_width(width);

        continue_game_button_texture = {
            renderer,
            {
                x,
                static_cast<int>(new_game_button_texture.get_y() + new_game_button_texture.get_h() * 1.5),
                width,
                button_texture.get_h(),
            },
        };
        const Texture::ScopedRender scoped_render = continue_game_button_texture.set_as_render_target();

        button_texture.render();

        Texture text_texture(renderer, Font::PRIMARY, "Continue", Color::WHITE);
        text_texture.set_position(
            (button_texture.get_w() - text_texture.get_w()) / 2,
            (button_texture.get_h() - text_texture.get_h()) / 2
        );
        text_texture.render();

        button_texture.destroy();
        text_texture.destroy();
    }
}

void init_main_menu_textures(SDL_Renderer *renderer, const int window_width, const int window_height) {
    init_big_mine_texture(renderer, window_width, window_height);
    init_title_texture(renderer, window_width);
    init_game_buttons(renderer, window_height);
}

// ReSharper disable once CppNotAllPathsReturnValue
Texture &get_main_menu_texture(const MainMenuTexture::Name name) {
    switch (name) {
        case MainMenuTexture::BIG_MINE: return big_mine_texture;
        case MainMenuTexture::TITLE: return title_texture;
        case MainMenuTexture::NEW_GAME_BUTTON: return new_game_button_texture;
        case MainMenuTexture::CONTINUE_GAME_BUTTON: return continue_game_button_texture;
    }
}

void free_main_menu_textures() {
    big_mine_texture.destroy();
    title_texture.destroy();
}
