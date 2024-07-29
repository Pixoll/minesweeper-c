#include "main_menu_textures.hpp"

#include <iostream>
#include <SDL.h>

#include "colors.hpp"
#include "fonts.hpp"
#include "texture.hpp"

Texture big_mine_texture;
Texture title_texture;

const auto big_mine_path = "assets/textures/mine_big.png";

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

void init_main_menu_textures(SDL_Renderer *renderer, const int window_width, const int window_height) {
    init_big_mine_texture(renderer, window_width, window_height);
    init_title_texture(renderer, window_width);
}

Texture &get_main_menu_texture(const MainMenuTexture::Name name) {
    switch (name) {
        case MainMenuTexture::BIG_MINE: return big_mine_texture;
        case MainMenuTexture::TITLE: return title_texture;
        default: {
            std::cerr << "Invalid texture name " << name << std::endl;
            exit(1);
        }
    }
}

void free_main_menu_textures() {
    big_mine_texture.destroy();
    title_texture.destroy();
}
