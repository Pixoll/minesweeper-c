#pragma once

#include <cstdlib>
#include <SDL.h>

struct Color {
    SDL_Color rgb{0, 0, 0, 0};
    Uint32 value = 0;

    Color() = default;

    Color(const SDL_Surface *surface, const char *hex_color) {
        if (hex_color[0] == '#')
            hex_color++;  // shift left once

        const int raw_rgb = strtol(hex_color, nullptr, 16);
        const Uint8 r = raw_rgb >> 16 & 0xff;
        const Uint8 g = raw_rgb >> 8 & 0xff;
        const Uint8 b = raw_rgb & 0xff;

        rgb = {r, g, b, 255};
        value = SDL_MapRGB(surface->format, r, g, b);
    }

    enum Name {
        BACKGROUND,
        THEME,

        GRID_1,
        GRID_2,
        GRID_3,
        GRID_4,
        GRID_5,
        GRID_6,
        GRID_7,
        GRID_8,

        FLAGGED_CELL,
        FLAGGED_CELL_BG,
        TRIGGERED_MINE,
        TRIGGERED_MINE_BG,

        BLACK,
        DARK_GREY,
        GREY,
        LIGHT_GREY,
        LIGHTER_GREY,
        WHITE,
    };

    static constexpr int NAMES_AMOUNT = WHITE + 1;
};

void init_colors(SDL_Window *window);
Color get_color(Color::Name name);
