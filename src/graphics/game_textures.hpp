#pragma once

#include <SDL.h>

#include "colors.hpp"
#include "fonts.hpp"
#include "texture.hpp"
#include "../core/game.hpp"

void init_game_textures(SDL_Renderer *renderer, const Game::Measurements &measurements);
Texture get_cell_texture(Texture::CellSubtype subtype, Texture::CellType type);
Texture get_cell_number_texture(int number);
Texture &get_game_texture(Texture::Name name);
void free_game_textures();
void update_text_texture(
    SDL_Renderer *renderer,
    Texture &texture,
    Font::Name font_name,
    Color::Name color,
    const char *text
);
