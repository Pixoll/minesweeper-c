#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <string>

#include "color.hpp"

constexpr SDL_Rect NULL_RECT = {0, 0, 0, 0};

class Texture {
    SDL_Renderer *const m_renderer = nullptr;
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
        SDL_Surface *surface = IMG_Load(image_path);
        m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);
        m_area = {0, 0, surface->w, surface->h};

        SDL_FreeSurface(surface);
    }

    Texture(SDL_Renderer *renderer, const char *image_path, const SDL_Rect area) : m_renderer(renderer), m_area(area) {
        SDL_Surface *surface = IMG_Load(image_path);
        m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);

        SDL_FreeSurface(surface);
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
        SDL_Surface *surface = TTF_RenderText_Blended(m_font, text.c_str(), m_font_color);
        m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);
        m_area = {position.x, position.y, surface->w, surface->h};

        SDL_FreeSurface(surface);
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

    void set_size(const int size) {
        m_area.h = size;
        m_area.w = size;
    }

    /**
     * Keeps the aspect ratio
     */
    void set_width(const float width) {
        const float factor = width / m_area.w;

        m_area.w *= factor;
        m_area.h *= factor;
    }

    /**
     * Keeps the aspect ratio
     */
    void set_height(const float height) {
        const float factor = height / m_area.h;

        m_area.w *= factor;
        m_area.h *= factor;
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
        SDL_Surface *surface = TTF_RenderText_Blended(m_font, text.c_str(), m_font_color);
        m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);
        m_area.h = surface->h;
        m_area.w = surface->w;

        SDL_FreeSurface(surface);
    }

    void render() const {
        SDL_RenderCopy(m_renderer, m_texture, nullptr, &m_area);
    }

    void render_from(const int x, const int y, const int w, const int h) const {
        const SDL_Rect source = {x, y, w, h};
        SDL_RenderCopy(m_renderer, m_texture, &source, nullptr);
    }

    void render_to(const int x, const int y) const {
        const SDL_Rect dest = {x, y, m_area.w, m_area.h};
        SDL_RenderCopy(m_renderer, m_texture, nullptr, &dest);
    }

    void render_moved(const int x, const int y) const {
        const SDL_Rect dest = {m_area.x + x, m_area.y + y, m_area.w, m_area.h};
        SDL_RenderCopy(m_renderer, m_texture, nullptr, &dest);
    }

    void destroy() {
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
