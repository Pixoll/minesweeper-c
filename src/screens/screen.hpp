#pragma once

#include <SDL.h>

class Screen {
public:
    virtual ~Screen() = default;
    virtual void before_event(const SDL_Event &event) = 0;
    virtual void on_keyboard_event(const SDL_KeyboardEvent &event) = 0;
    virtual void on_mouse_button_event(const SDL_MouseButtonEvent &event) = 0;
    virtual void on_mouse_motion_event(const SDL_MouseMotionEvent &event) = 0;
    virtual void on_mouse_wheel_event(const SDL_MouseWheelEvent &event) = 0;
    virtual void on_quit_event(const SDL_QuitEvent &event) = 0;
    virtual void render() = 0;
};
