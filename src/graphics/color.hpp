#pragma once

#include <cstdlib>
#include <SDL.h>

class Color {
public:
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

        FLAG,
        FLAGGED_CELL,
        TRIGGERED_MINE,
        TRIGGERED_CELL,

        BLACK,
        DARK_GREY,
        GREY,
        LIGHT_GREY,
        LIGHTER_GREY,
        WHITE,
    };

private:
    static constexpr int NAMES = WHITE + 1;

    static Color colors[NAMES];

    SDL_Color m_rgb{0, 0, 0, 0};
    Uint32 m_value = 0;

public:
    Color() = default;

    Color(const SDL_Surface *surface, const char *hex_color) {
        if (hex_color[0] == '#')
            hex_color++;  // shift left once

        const int raw_rgb = strtol(hex_color, nullptr, 16);
        const Uint8 r = raw_rgb >> 16 & 0xff;
        const Uint8 g = raw_rgb >> 8 & 0xff;
        const Uint8 b = raw_rgb & 0xff;

        m_rgb = {r, g, b, 255};
        m_value = SDL_MapRGB(surface->format, r, g, b);
    }

    [[nodiscard]] SDL_Color get_rgb() const {
        return m_rgb;
    }

    [[nodiscard]] Uint32 get_value() const {
        return m_value;
    }

    [[nodiscard]] static Color get(const Name name) {
        return colors[name];
    }

    static void make(SDL_Window *window) {
        const SDL_Surface *surface = SDL_GetWindowSurface(window);

        colors[THEME] = {surface, "#d77f37"};
        colors[BACKGROUND] = {surface, "#333333"};

        colors[GRID_1] = {surface, "#b3b3ff"};  // #0000ff
        colors[GRID_2] = {surface, "#b3ffb3"};  // #008000
        colors[GRID_3] = {surface, "#ffb3b3"};  // #ff0000
        colors[GRID_4] = {surface, "#4d4dff"};  // #000080
        colors[GRID_5] = {surface, "#ff4d4d"};  // #800000
        colors[GRID_6] = {surface, "#b3ffff"};  // #008080
        colors[GRID_7] = {surface, "#bfbfbf"};  // #808080
        colors[GRID_8] = {surface, "#ffffff"};

        colors[FLAG] = {surface, "#333333"};
        colors[FLAGGED_CELL] = {surface, "#606060"};
        colors[TRIGGERED_MINE] = {surface, "#431a0d"};
        colors[TRIGGERED_CELL] = {surface, "#b6350d"};

        colors[BLACK] = {surface, "#000000"};
        colors[DARK_GREY] = {surface, "#1e1f1c"};
        colors[GREY] = {surface, "#333333"};
        colors[LIGHT_GREY] = {surface, "#606060"};
        colors[LIGHTER_GREY] = {surface, "#cfcfcf"};
        colors[WHITE] = {surface, "#ffffff"};
    }
};

Color Color::colors[] = {};
