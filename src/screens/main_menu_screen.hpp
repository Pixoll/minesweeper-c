#pragma once

#include <SDL.h>

#include "game_screen.hpp"
#include "screen.hpp"

class Engine;

class MainMenuScreen final : virtual public Screen {
    Engine *m_engine;

public:
    explicit MainMenuScreen(Engine *engine) : m_engine(engine) {}

    ~MainMenuScreen() override = default;

    void run_logic(const SDL_Event &event) override {
        if (event.type != SDL_MOUSEBUTTONDOWN)
            return;

        if (event.button.button != SDL_BUTTON_LEFT)
            return;

        m_engine->set_screen<GameScreen>(m_engine);
    }

    void render() override {}
};
