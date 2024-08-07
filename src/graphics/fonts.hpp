#pragma once

#include <cstdlib>
#include <iostream>
#include <memory>
#include <SDL_ttf.h>

class Font {
public:
    enum Type {
        RUBIK_LIGHT,
        RUBIK_MEDIUM,
        RUBIK_REGULAR,
        RUBIK_BOLD,
    };

    enum NameShared {
        PRIMARY,
        SECONDARY,
    };

    using Shared = std::shared_ptr<Font>;

private:
    static constexpr int TYPES = RUBIK_BOLD + 1;
    static constexpr int NAMES = SECONDARY + 1;

    static constexpr const char *FONT_PATHS[TYPES] = {
        "assets/fonts/Rubik-Light.ttf",
        "assets/fonts/Rubik-Medium.ttf",
        "assets/fonts/Rubik-Regular.ttf",
        "assets/fonts/Rubik-Bold.ttf",
    };

    static Shared shared_fonts[NAMES];

    TTF_Font *m_font = nullptr;
    int m_size = 0;

public:
    Font() = default;

    Font(const Type type, const int size) : m_size(size) {
        m_font = TTF_OpenFont(FONT_PATHS[type], m_size);
        if (m_font == nullptr) {
            std::cerr << "Error while loading font ID " << type << " at TTF_OpenFont(): " << SDL_GetError()
                    << std::endl;
            exit(1);
        }
    }

    ~Font() {
        TTF_CloseFont(m_font);
    }

    [[nodiscard]] TTF_Font *get_font() const {
        return m_font;
    }

    [[nodiscard]] int get_size() const {
        return m_size;
    }

    void static init_shared(const int window_height) {
        shared_fonts[PRIMARY] = std::make_shared<Font>(RUBIK_REGULAR, window_height * 0.025);
        shared_fonts[SECONDARY] = std::make_shared<Font>(RUBIK_REGULAR, window_height * 0.02);
    }

    [[nodiscard]] static Shared get_shared(const NameShared name) {
        return shared_fonts[name];
    }

    static void free_shared() {
        for (Shared font : shared_fonts)
            font.reset();
    }
};

Font::Shared Font::shared_fonts[] = {};
