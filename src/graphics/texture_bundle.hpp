#pragma once

#include <memory>
#include <vector>

#include "texture.hpp"

class TextureBundle {
    std::vector<std::shared_ptr<Texture>> m_textures{};

public:
    TextureBundle() = default;

    ~TextureBundle() = default;

    void add(const std::shared_ptr<Texture> &texture) {
        m_textures.push_back(texture);
    }

    void remove(const int index) {
        m_textures.erase(m_textures.begin() + index);
    }

    void render() const {
        for (const auto &texture : m_textures)
            texture->render();
    }

    std::shared_ptr<Texture> operator[](const int index) const {
        return m_textures[index];
    }
};
