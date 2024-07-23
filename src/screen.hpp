#pragma once

#include <SDL.h>

class Screen {
public:
    virtual ~Screen() = default;
    virtual void run_logic(const SDL_Event &event) = 0;
    virtual void render() = 0;
};
