#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL2_rotozoom.h>
#include <string>

#include "color.hpp"

constexpr SDL_Rect NULL_RECT = {0, 0, 0, 0};

class Texture {
    SDL_Renderer *const m_renderer = nullptr;
    SDL_Surface *m_surface = nullptr;
    SDL_Texture *m_texture = nullptr;
    SDL_Rect m_area{0, 0, 0, 0};
    TTF_Font *const m_font = nullptr;
    const SDL_Color m_font_color{0, 0, 0, 0};

public:
    /**
     * Automatically releases the render target upon scope exit
     */
    class ScopedRender {
        SDL_Renderer *m_renderer;

    public:
        ScopedRender(
            SDL_Renderer *renderer,
            SDL_Texture *texture,
            const SDL_BlendMode blend_mode
        ) : m_renderer(renderer) {
            SDL_SetRenderTarget(m_renderer, texture);
            SDL_SetTextureBlendMode(texture, blend_mode);
        }

        ~ScopedRender() {
            release();
        }

        void release() const {
            SDL_SetRenderTarget(m_renderer, nullptr);
        }
    };

    Texture() = default;

    Texture(
        SDL_Renderer *renderer,
        const SDL_Rect area,
        const int access = SDL_TEXTUREACCESS_TARGET
    ) : m_renderer(renderer),
        m_area(area) {
        const Uint32 pixel_format = SDL_GetWindowPixelFormat(SDL_RenderGetWindow(m_renderer));
        m_texture = SDL_CreateTexture(m_renderer, pixel_format, access, m_area.w, m_area.h);
    }

    Texture(SDL_Renderer *renderer, const char *image_path) : m_renderer(renderer) {
        m_surface = IMG_Load(image_path);
        m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
        m_area = {0, 0, m_surface->w, m_surface->h};
    }

    Texture(SDL_Renderer *renderer, const char *image_path, const SDL_Rect area) : m_renderer(renderer), m_area(area) {
        m_surface = IMG_Load(image_path);
        m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
        m_area.w = m_surface->w;
        m_area.h = m_surface->h;
        scale(static_cast<double>(area.w) / m_area.w, static_cast<double>(area.h) / m_area.h);
    }

    Texture(
        SDL_Renderer *renderer,
        TTF_Font *font,
        const std::string &text,
        const Color::Name color,
        const SDL_Point position = {0, 0}
    ) : m_renderer(renderer),
        m_font(font),
        m_font_color(Color::get(color).get_rgb()) {
        m_surface = TTF_RenderText_Blended(m_font, text.c_str(), m_font_color);
        m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
        m_area = {position.x, position.y, m_surface->w, m_surface->h};
    }

    ~Texture() {
        destroy();
    }

    [[nodiscard]] int get_x() const {
        return m_area.x;
    }

    [[nodiscard]] int get_y() const {
        return m_area.y;
    }

    [[nodiscard]] int get_w() const {
        return m_area.w;
    }

    [[nodiscard]] int get_h() const {
        return m_area.h;
    }

    [[nodiscard]] const SDL_Rect &get_area() const {
        return m_area;
    }

    [[nodiscard]] bool contains(const SDL_Point point) const {
        const auto [x, y] = point;
        const auto [ax, ay, w, h] = m_area;
        return x >= ax && x <= ax + w && y >= ay && y <= ay + h;
    }

    [[nodiscard]] bool contains_moved(const int texture_dx, const int texture_dy, const SDL_Point point) const {
        const auto [px, py] = point;
        const auto [x, y, w, h] = m_area;
        return px >= x + texture_dx && px <= x + texture_dx + w && py >= y + texture_dy && py <= y + texture_dy + h;
    }

    void scale(const double zoom_x, const double zoom_y) {
        if (zoom_x == 1 && zoom_y == 1)
            return;

        SDL_Surface *new_surface;

        if (zoom_x < 1 && zoom_y < 1) {
            const double factor_x = 1 / zoom_x;
            const double factor_y = 1 / zoom_y;
            const int factor_x_int = factor_x;
            const int factor_y_int = factor_y;

            if (factor_x != factor_x_int || factor_x_int != factor_y_int) {
                m_area.w *= zoom_x;
                m_area.h *= zoom_y;
                return;
            }

            new_surface = shrinkSurface(m_surface, factor_x_int, factor_y_int);
        } else {
            new_surface = zoomSurface(m_surface, zoom_x, zoom_x, SMOOTHING_ON);
        }

        destroy();
        m_surface = new_surface;
        m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
        m_area.w = m_surface->w;
        m_area.h = m_surface->h;
    }

    /**
     * Keeps the aspect ratio
     */
    void set_width(const double width) {
        const double zoom = width / m_area.w;
        scale(zoom, zoom);
    }

    /**
     * Keeps the aspect ratio
     */
    void set_height(const double height) {
        const double zoom = height / m_area.h;
        scale(zoom, zoom);
    }

    void set_position(const int x, const int y) {
        m_area.x = x;
        m_area.y = y;
    }

    void set_x(const int x) {
        m_area.x = x;
    }

    void set_color(const SDL_Color color) const {
        SDL_SetTextureColorMod(m_texture, color.r, color.g, color.b);
    }

    void set_color(const Color::Name color) const {
        set_color(Color::get(color).get_rgb());
    }

    [[nodiscard]] ScopedRender set_as_render_target(const SDL_BlendMode blend_mode = SDL_BLENDMODE_BLEND) const {
        return {m_renderer, m_texture, blend_mode};
    }

    void update_text(const std::string &text) {
        destroy();
        m_surface = TTF_RenderText_Blended(m_font, text.c_str(), m_font_color);
        m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
        m_area.h = m_surface->h;
        m_area.w = m_surface->w;
    }

    void render() const {
        SDL_RenderCopy(m_renderer, m_texture, nullptr, &m_area);
    }

    void render_from(const int x, const int y, const int w, const int h) const {
        const SDL_Rect source = {x, y, w, h};
        SDL_RenderCopy(m_renderer, m_texture, &source, nullptr);
    }

    void render_to(const int x, const int y) const {
        const SDL_Rect destination = {x, y, m_area.w, m_area.h};
        SDL_RenderCopy(m_renderer, m_texture, nullptr, &destination);
    }

    void render_moved(const int x, const int y) const {
        const SDL_Rect destination = {m_area.x + x, m_area.y + y, m_area.w, m_area.h};
        SDL_RenderCopy(m_renderer, m_texture, nullptr, &destination);
    }

    void destroy() {
        SDL_FreeSurface(m_surface);
        m_surface = nullptr;

        if (m_texture == nullptr)
            return;

        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }

private:
    static bool is_null_rect(const SDL_Rect &rect) {
        return rect.x == 0 && rect.y == 0 && rect.h == 0 && rect.w == 0;
    }
};
