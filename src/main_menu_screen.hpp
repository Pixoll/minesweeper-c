#pragma once

#include <SDL.h>

#include "game_screen.hpp"
#include "screen.hpp"

class Game;

class MainMenuScreen final : virtual public Screen {
    Game *m_game;

public:
    explicit MainMenuScreen(Game *game) : m_game(game) {}

    void run_logic(const SDL_Event &event) override {
        if (event.type != SDL_MOUSEBUTTONDOWN)
            return;

        if (event.button.button != SDL_BUTTON_LEFT)
            return;

        m_game->set_screen(new GameScreen(m_game));
    }

    void render() override {}
};
