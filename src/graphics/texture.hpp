#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include "colors.hpp"
#include "fonts.hpp"

constexpr SDL_Rect NULL_RECT = {0, 0, 0, 0};

class Texture {
    SDL_Surface *m_surface = nullptr;
    SDL_Texture *m_texture = nullptr;
    SDL_Rect m_area{0, 0, 0, 0};

public:
    Texture() = default;

    Texture(SDL_Surface *surface, SDL_Texture *texture, const SDL_Rect area) :
        m_surface(surface),
        m_texture(texture),
        m_area(area) {}

    Texture(SDL_Texture *texture, const SDL_Rect area) : Texture(nullptr, texture, area) {}

    Texture(SDL_Renderer *renderer, const SDL_Rect area, const int access = SDL_TEXTUREACCESS_TARGET) : m_area(area) {
        const Uint32 pixel_format = SDL_GetWindowSurface(SDL_RenderGetWindow(renderer))->format->format;
        m_texture = SDL_CreateTexture(renderer, pixel_format, access, m_area.w, m_area.h);
    }

    Texture(SDL_Renderer *renderer, const char *image_path) {
        m_surface = IMG_Load(image_path);
        m_texture = SDL_CreateTextureFromSurface(renderer, m_surface);
        m_area = {0, 0, m_surface->w, m_surface->h};
    }

    Texture(SDL_Renderer *renderer, const char *image_path, const SDL_Rect area) : m_area(area) {
        m_surface = IMG_Load(image_path);
        m_texture = SDL_CreateTextureFromSurface(renderer, m_surface);
    }

    Texture(
        SDL_Renderer *renderer,
        const Font::Name font,
        const char *text,
        const Color::Name color,
        const SDL_Point position = {0, 0}
    ) {
        m_surface = TTF_RenderText_Solid(get_font(font).font, text, get_color(color).rgb);
        m_texture = SDL_CreateTextureFromSurface(renderer, m_surface);
        m_area = {position.x, position.y, m_surface->w, m_surface->h};
    }

    ~Texture() = default;

    [[nodiscard]] int get_y() const {
        return m_area.y;
    }

    [[nodiscard]] int get_x() const {
        return m_area.x;
    }

    [[nodiscard]] int get_h() const {
        return m_area.h;
    }

    [[nodiscard]] int get_w() const {
        return m_area.w;
    }

    [[nodiscard]] const SDL_Rect &get_area() const {
        return m_area;
    }

    void set_size(const int size) {
        m_area.h = size;
        m_area.w = size;
    }

    void set_position(const int x, const int y) {
        m_area.x = x;
        m_area.y = y;
    }

    void move(const int x_pixels, const int y_pixels) {
        m_area.x += x_pixels;
        m_area.y += y_pixels;
    }

    void set_color_mod(const SDL_Color color) const {
        SDL_SetTextureColorMod(m_texture, color.r, color.g, color.b);
    }

    void set_color_mod(const Color::Name color) const {
        set_color_mod(get_color(color).rgb);
    }

    void set_as_render_target(SDL_Renderer *renderer, const SDL_BlendMode blend_mode = SDL_BLENDMODE_BLEND) const {
        SDL_SetRenderTarget(renderer, m_texture);
        SDL_SetTextureBlendMode(m_texture, blend_mode);
    }

    void render(
        SDL_Renderer *renderer,
        const SDL_Rect source = NULL_RECT,
        const SDL_Rect destination = NULL_RECT
    ) const {
        SDL_RenderCopy(
            renderer,
            m_texture,
            is_null_rect(source) ? nullptr : &source,
            is_null_rect(destination)
            ? is_null_rect(source)
              ? &m_area
              : nullptr
            : &destination
        );
    }

    void destroy() {
        if (m_texture == nullptr)
            return;

        SDL_FreeSurface(m_surface);
        SDL_DestroyTexture(m_texture);
        m_surface = nullptr;
        m_texture = nullptr;
    }

private:
    static bool is_null_rect(const SDL_Rect &rect) {
        return rect.x == 0 && rect.y == 0 && rect.h == 0 && rect.w == 0;
    }
};
