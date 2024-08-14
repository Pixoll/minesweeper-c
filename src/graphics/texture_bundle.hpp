#pragma once

#include <memory>
#include <SDL.h>
#include <vector>

#include "texture.hpp"

class TextureBundle {
    std::vector<std::shared_ptr<Texture>> m_textures{};
    SDL_Point m_position{0, 0};

public:
    TextureBundle() = default;
    TextureBundle(const TextureBundle &bundle) = default;

    ~TextureBundle() = default;

    void add(const std::shared_ptr<Texture> &texture) {
        m_textures.push_back(texture);
    }

    void remove(const int index) {
        m_textures.erase(m_textures.begin() + index);
    }

    void render() const {
        for (const auto &texture : m_textures)
            texture->render_moved(m_position.x, m_position.y);
    }

    std::shared_ptr<Texture> &operator[](const int index) {
        return m_textures[index];
    }

    [[nodiscard]] std::shared_ptr<Texture> last() const {
        return m_textures.back();
    }

    [[nodiscard]] int get_y() const {
        return m_position.y;
    }

    [[nodiscard]] int get_h() const {
        int height = 0;

        for (const auto &texture : m_textures) {
            const int temp_height = texture->get_y() + texture->get_h();

            if (temp_height > height)
                height = temp_height;
        }

        return height;
    }

    void set_x(const int x) {
        m_position.x = x;
    }

    void set_y(const int y) {
        m_position.y = y;
    }
};
