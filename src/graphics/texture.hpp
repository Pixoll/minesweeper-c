#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include "colors.hpp"
#include "fonts.hpp"

constexpr SDL_Rect NULL_RECT = {0, 0, 0, 0};

class Texture {
    SDL_Renderer *m_renderer = nullptr;
    SDL_Surface *m_surface = nullptr;
    SDL_Texture *m_texture = nullptr;
    SDL_Rect m_area{0, 0, 0, 0};
    TTF_Font *m_font = nullptr;
    SDL_Color m_font_color{0, 0, 0, 0};

public:
    Texture() = default;

    Texture(
        SDL_Renderer *renderer,
        const SDL_Rect area,
        const int access = SDL_TEXTUREACCESS_TARGET
    ) : m_renderer(renderer), m_area(area) {
        destroy();
        const Uint32 pixel_format = SDL_GetWindowSurface(SDL_RenderGetWindow(m_renderer))->format->format;
        m_texture = SDL_CreateTexture(m_renderer, pixel_format, access, m_area.w, m_area.h);
    }

    Texture(SDL_Renderer *renderer, const char *image_path) : m_renderer(renderer) {
        destroy();
        m_surface = IMG_Load(image_path);
        m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
        m_area = {0, 0, m_surface->w, m_surface->h};
    }

    Texture(SDL_Renderer *renderer, const char *image_path, const SDL_Rect area) : m_renderer(renderer), m_area(area) {
        destroy();
        m_surface = IMG_Load(image_path);
        m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
    }

    Texture(
        SDL_Renderer *renderer,
        const Font::Name font,
        const char *text,
        const Color::Name color,
        const SDL_Point position = {0, 0}
    ) : m_renderer(renderer), m_font(get_font(font).font), m_font_color(get_color(color).rgb) {
        destroy();
        m_surface = TTF_RenderText_Blended(m_font, text, m_font_color);
        m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
        m_area = {
            m_area.x == 0 ? position.x : m_area.x,
            m_area.y == 0 ? position.y : m_area.y,
            m_surface->w,
            m_surface->h,
        };
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

    void scale_by(const float factor) {
        m_area.h *= factor;
        m_area.w *= factor;
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

    void set_as_render_target(const SDL_BlendMode blend_mode = SDL_BLENDMODE_BLEND) const {
        SDL_SetRenderTarget(m_renderer, m_texture);
        SDL_SetTextureBlendMode(m_texture, blend_mode);
    }

    void update_text(const char *text) {
        destroy();
        m_surface = TTF_RenderText_Blended(m_font, text, m_font_color);
        m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
        m_area.h = m_surface->h;
        m_area.w = m_surface->w;
    }

    void render(const SDL_Rect source = NULL_RECT, const SDL_Point destination = {0, 0}) const {
        const bool is_source_null = is_null_rect(source);

        if (destination.x == 0 && destination.y == 0) {
            SDL_RenderCopy(
                m_renderer,
                m_texture,
                is_source_null ? nullptr : &source,
                is_source_null ? &m_area : nullptr
            );

            return;
        }

        const SDL_Rect dest = {destination.x, destination.y, m_area.w, m_area.h};

        SDL_RenderCopy(
            m_renderer,
            m_texture,
            is_source_null ? nullptr : &source,
            &dest
        );
    }

    void render(const int x, const int y) const {
        render(NULL_RECT, {x, y});
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
