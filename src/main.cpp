#include <cstdlib>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "game.hpp"
#include "main_menu_screen.hpp"

GameParameters start_sdl();
void quit_sdl(SDL_Renderer *renderer, SDL_Window *window);
void throw_sdl_error(const char *function_name, int code = 0);

int main(int argc, char *argv[]) {
    const GameParameters parameters = start_sdl();

    Game game(parameters);
    game.set_screen(new MainMenuScreen(&game));
    game.run();

    quit_sdl(parameters.renderer, parameters.window);

    return 0;
}

GameParameters start_sdl() {
    const int sdl_init_error = SDL_Init(SDL_INIT_VIDEO);
    if (sdl_init_error < 0)
        throw_sdl_error("SDL_Init", sdl_init_error);

    SDL_Window *window = SDL_CreateWindow(
        "Minesweeper",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        720,
        SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE
    );

    if (window == nullptr)
        throw_sdl_error("SDL_CreateWindow");

    SDL_Surface *icon = IMG_Load("assets/images/icon.png");
    SDL_SetWindowIcon(window, icon);
    SDL_FreeSurface(icon);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == nullptr)
        throw_sdl_error("SDL_CreateRenderer");

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    const int ttf_ready = TTF_Init();
    if (ttf_ready == -1)
        throw_sdl_error("TTF_Init");

    SDL_DisplayMode current_display_mode;
    const int display_mode_error = SDL_GetCurrentDisplayMode(0, &current_display_mode);
    if (display_mode_error < 0)
        throw_sdl_error("SDL_GetCurrentDisplayMode", display_mode_error);

    const long render_interval_microsecs = 1000000 / current_display_mode.refresh_rate;

    return {window, renderer, render_interval_microsecs};
}

void quit_sdl(SDL_Renderer *renderer, SDL_Window *window) {
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void throw_sdl_error(const char *function_name, const int code) {
    using std::cerr, std::endl;

    cerr << "Error ";

    if (code != 0)
        cerr << code << " ";

    cerr << "at " << function_name << "(): " << SDL_GetError() << endl;
    exit(1);
}
