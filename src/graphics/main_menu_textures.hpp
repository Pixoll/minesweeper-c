#pragma once

#include <SDL.h>

#include "texture.hpp"

class MainMenuTexture {
public:
    enum Name {
        BIG_MINE,
        TITLE,
        NEW_GAME_BUTTON,
        CONTINUE_GAME_BUTTON,
    };
};

void init_main_menu_textures(SDL_Renderer *renderer, int window_width, int window_height);

Texture &get_main_menu_texture(MainMenuTexture::Name name);

void free_main_menu_textures();
