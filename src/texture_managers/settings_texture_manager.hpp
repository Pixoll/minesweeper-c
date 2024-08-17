#pragma once

#include <memory>
#include <SDL.h>
#include <string>
#include <vector>

#include "../graphics/color.hpp"
#include "../graphics/font.hpp"
#include "../graphics/texture.hpp"
#include "../graphics/texture_bundle.hpp"

class SettingsTextureManager {
public:
    enum TextureName {
        BACK_BUTTON,
        TOGGLE_OFF,
        TOGGLE_ON,
        SCROLLBAR,
    };

    enum TextureBundleName {
        SETTING_SHOW_CELL_BORDERS,
        SETTING_SHOW_CONTROLS,
        SETTING_SWAP_CONTROLS,
        SETTING_SINGLE_CLICK_CONTROLS,
        SETTING_EASY_DIG,
        SETTING_EASY_FLAG,
    };

    using SettingsTexture = std::shared_ptr<Texture>;
    using SettingsTextureBundle = std::shared_ptr<TextureBundle>;

private:
    struct SettingNameAndDescription {
        const char *name;
        const char *description;
    };

    static constexpr auto BACK_BUTTON_IMAGE_PATH = "assets/textures/button_back.png";
    static constexpr auto TOGGLE_OFF_IMAGE_PATH = "assets/textures/toggle_off.png";
    static constexpr auto TOGGLE_ON_IMAGE_PATH = "assets/textures/toggle_on.png";
    static constexpr auto SCROLLBAR_IMAGE_PATH = "assets/textures/scrollbar.png";

    static constexpr int SETTINGS_AMOUNT = SETTING_EASY_FLAG + 1;
    static constexpr SettingNameAndDescription SETTINGS_NAMES_AND_DESCRIPTIONS[SETTINGS_AMOUNT] = {
        {
            "Show cell borders",
            "Choose whether to display cell borders or not.",
        },
        {
            "Show controls",
            "Choose whether to display the controls in the corner of the game or not.",
        },
        {
            "Swap controls",
            "Choose whether to swap the game controls or not.",
        },
        {
            "Single click controls",
            "Play the game with only one button. An action toggle will be shown at the border of the screen,\n"
            "allowing you to switch between dig and flag modes.",
        },
        {
            "Easy digging",
            "Clicking a number will dig all of its surrounding unflagged cells with one click.\n"
            "It will work if the amount of surrounding flagged cells matches the clicked digit."
        },
        {
            "Easy flagging",
            "Clicking a number will flag all of its surrounding covered cells with one click.\n"
            "It will work if the amount of surrounding closed covered matches the clicked digit.",
        },
    };

    SDL_Renderer *m_renderer;
    const int m_window_width;
    const int m_window_height;
    const int m_window_padding;
    int m_settings_total_height = 0;

    SettingsTexture m_back_button_texture;
    SettingsTexture m_scrollbar_texture;

    SettingsTexture m_toggle_off_texture;
    SettingsTexture m_toggle_on_texture;

    SettingsTextureBundle m_text_textures_bundles[SETTINGS_AMOUNT];

public:
    SettingsTextureManager(SDL_Renderer *renderer, const int window_width, const int window_height) :
        m_renderer(renderer),
        m_window_width(window_width),
        m_window_height(window_height),
        m_window_padding(window_height * 0.025) {
        make_back_button_texture();
        make_toggles_textures();
        make_setting_text_texture_bundles();
        make_scrollbar_texture();
    }

    ~SettingsTextureManager() = default;

    [[nodiscard]] SettingsTexture get(const TextureName name) const {
        switch (name) {
            case BACK_BUTTON: return m_back_button_texture;
            case TOGGLE_OFF: return m_toggle_off_texture;
            case TOGGLE_ON: return m_toggle_on_texture;
            case SCROLLBAR: return m_scrollbar_texture;
        }
        __builtin_unreachable();
    }

    [[nodiscard]] SettingsTextureBundle get(const TextureBundleName bundle_name) const {
        return m_text_textures_bundles[bundle_name];
    }

    [[nodiscard]] int get_settings_total_height() const {
        return m_settings_total_height;
    }

private:
    void make_back_button_texture() {
        const int height = Font::get_shared(Font::PRIMARY)->get_size();
        m_back_button_texture = std::make_shared<Texture>(m_renderer, BACK_BUTTON_IMAGE_PATH);
        m_back_button_texture->set_position(m_window_padding, m_window_padding);
        m_back_button_texture->set_height(height);
    }

    void make_toggles_textures() {
        const int toggle_size = m_window_height * 0.1;
        const int x = (m_window_width - toggle_size) / 2;
        const Font font(Font::RUBIK_LIGHT, m_window_height * 0.025);

        m_toggle_off_texture = std::make_shared<Texture>(m_renderer, SDL_Rect{x, 0, toggle_size, toggle_size});
        const Texture::ScopedRender off_texture_scoped_render = m_toggle_off_texture->set_as_render_target();

        const Texture off_texture(
            m_renderer,
            TOGGLE_OFF_IMAGE_PATH,
            SDL_Rect{0, 0, toggle_size, toggle_size}
        );
        off_texture.set_color(Color::LIGHT_GREY);
        off_texture.render();

        Texture text_off_texture(m_renderer, font.get_font(), "OFF", Color::WHITE);
        text_off_texture.set_position(
            (toggle_size - text_off_texture.get_w()) / 2,
            (toggle_size - text_off_texture.get_h()) / 2
        );
        text_off_texture.render();
        off_texture_scoped_render.release();

        m_toggle_on_texture = std::make_shared<Texture>(m_renderer, SDL_Rect{x, 0, toggle_size, toggle_size});
        const Texture::ScopedRender on_texture_scoped_render = m_toggle_on_texture->set_as_render_target();

        const Texture on_texture(
            m_renderer,
            TOGGLE_ON_IMAGE_PATH,
            SDL_Rect{0, 0, toggle_size, toggle_size}
        );
        on_texture.set_color(Color::THEME);
        on_texture.render();

        Texture text_on_texture(m_renderer, font.get_font(), "ON", Color::BACKGROUND);
        text_on_texture.set_position(
            (toggle_size - text_on_texture.get_w()) / 2,
            (toggle_size - text_on_texture.get_h()) / 2
        );
        text_on_texture.render();
        on_texture_scoped_render.release();
    }

    void make_setting_text_texture_bundles() {
        const int bundles_padding = Font::get_shared(Font::SECONDARY)->get_size()
                + m_toggle_on_texture->get_h() * 1.5;
        const int y_first = m_window_height * 0.15;

        for (int bundle_name = 0; bundle_name < SETTINGS_AMOUNT; ++bundle_name) {
            const auto &[name, description] = SETTINGS_NAMES_AND_DESCRIPTIONS[bundle_name];

            m_text_textures_bundles[bundle_name] = std::make_shared<TextureBundle>(
                make_setting_texture_bundle(name, description)
            );

            if (bundle_name == 0) {
                m_text_textures_bundles[bundle_name]->set_y(y_first);
                continue;
            }

            const SettingsTextureBundle previous_texture_bundle = get(static_cast<TextureBundleName>(bundle_name - 1));

            m_text_textures_bundles[bundle_name]->set_y(
                previous_texture_bundle->get_y()
                + previous_texture_bundle->get_h()
                + bundles_padding
            );
        }

        const SettingsTextureBundle last_texture_bundle = m_text_textures_bundles[SETTINGS_AMOUNT - 1];

        m_settings_total_height = last_texture_bundle->get_y()
                + last_texture_bundle->get_h()
                - y_first;
    }

    void make_scrollbar_texture() {
        const int x = m_window_width * 0.996;
        const int width = m_window_width - x;
        const int height = m_window_height * m_window_height / m_settings_total_height;

        m_scrollbar_texture = std::make_shared<Texture>(m_renderer, SDL_Rect{x, 0, width, height});
        const Texture::ScopedRender scoped_render = m_scrollbar_texture->set_as_render_target();

        const Texture scrollbar_image_texture(m_renderer, SCROLLBAR_IMAGE_PATH, {0, 0, width, height});
        scrollbar_image_texture.set_color(Color::LIGHTER_GREY);
        scrollbar_image_texture.render();
    }

    TextureBundle make_setting_texture_bundle(const char *name, const std::string &description) const {
        using std::string;

        TextureBundle texture_bundle;

        const Font::Shared primary_font = Font::get_shared(Font::PRIMARY);

        const auto name_texture = std::make_shared<Texture>(
            m_renderer,
            primary_font->get_font(),
            name,
            Color::WHITE
        );

        texture_bundle.add(name_texture);

        TTF_Font *const description_font = Font::get_shared(Font::SECONDARY)->get_font();

        std::vector<string> description_lines{""};
        int description_width = 0;
        int description_height;

        for (const char c : description) {
            if (c != '\n') {
                string &line = description_lines[description_lines.size() - 1];
                line += c;

                int line_width;
                TTF_SizeText(description_font, line.c_str(), &line_width, &description_height);

                if (line_width > description_width)
                    description_width = line_width;

                continue;
            }

            description_lines.emplace_back("");
        }

        name_texture->set_x((description_width - name_texture->get_w()) / 2);

        const auto description_texture = std::make_shared<Texture>(
            m_renderer,
            description_font,
            description_lines[0],
            Color::LIGHTER_GREY,
            SDL_Point{0, name_texture->get_h() + primary_font->get_size() / 2}
        );

        description_texture->set_x((description_width - description_texture->get_w()) / 2);
        texture_bundle.add(description_texture);
        description_lines.erase(description_lines.begin());

        for (const auto &line : description_lines) {
            const int last_y = texture_bundle.last()->get_y();

            const auto line_texture = std::make_shared<Texture>(
                m_renderer,
                description_font,
                line,
                Color::LIGHTER_GREY,
                SDL_Point{0, last_y + description_height}
            );

            line_texture->set_x((description_width - line_texture->get_w()) / 2);
            texture_bundle.add(line_texture);
        }

        texture_bundle.set_x((m_window_width - description_width) / 2);

        return texture_bundle;
    }
};
