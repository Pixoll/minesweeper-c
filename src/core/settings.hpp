#pragma once

#include <filesystem>
#include <fstream>

class Settings {
public:
    enum Name {
        SHOW_CELL_BORDERS,
        SHOW_CONTROLS,
        SWAP_CONTROLS,
        SINGLE_CLICK_CONTROLS,
        EASY_DIG,
        EASY_FLAG,
    };

private:
    using ofstream = std::ofstream;
    using ifstream = std::ifstream;
    using ios = std::ios;

    static constexpr int SETTINGS_AMOUNT = EASY_FLAG + 1;
    static constexpr auto SETTINGS_PATH = "settings.bin";
    static constexpr bool DEFAULTS[SETTINGS_AMOUNT] = {
        false,
        true,
        false,
        false,
        true,
        true,
    };

    static bool settings[SETTINGS_AMOUNT];

public:
    static void load() {
        if (!std::filesystem::exists(SETTINGS_PATH)) {
            ofstream settings_file(SETTINGS_PATH, ios::binary | ios::out);

            for (int i = 0; i < SETTINGS_AMOUNT; ++i) {
                settings[i] = DEFAULTS[i];
                settings_file.write(reinterpret_cast<const char *>(&settings[i]), sizeof(settings[i]));
            }

            settings_file.close();

            return;
        }

        ifstream settings_file(SETTINGS_PATH, ios::binary | ios::in);

        for (auto &setting : settings)
            settings_file.read(reinterpret_cast<char *>(&setting), sizeof(setting));

        settings_file.close();
    }

    static bool is_on(const Name name) {
        return settings[name];
    }

    static void toggle(const Name name) {
        settings[name] = !settings[name];
    }

    static void save() {
        ofstream settings_file(SETTINGS_PATH, ios::binary | ios::out);

        for (const auto &setting : settings)
            settings_file.write(reinterpret_cast<const char *>(&setting), sizeof(setting));

        settings_file.close();
    }
};

bool Settings::settings[SETTINGS_AMOUNT] = {};
