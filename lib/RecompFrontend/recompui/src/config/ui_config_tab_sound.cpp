#include "librecomp/config.hpp"
#include "recompui/config.h"

namespace recompui {

static bool created_sound_config = false;

namespace config {
    recomp::config::Config &get_sound_config() {
        if (!created_sound_config) {
            throw std::runtime_error("sound config has not been created yet. Call create_sound_tab() first.");
        }
        return config::get_config(config::sound::id);
    }

    template <typename T = uint32_t>
    T get_sound_config_number_value(const std::string& option_id) {
        return static_cast<T>(std::get<double>(get_sound_config().get_option_value(option_id)));
    }

    double sound::get_main_volume() {
        return get_sound_config_number_value<double>(sound::options::main_volume);
    }

    recomp::config::Config &create_sound_tab(const std::string &name) {
        created_sound_config = true;
        recomp::config::Config &config = recompui::config::create_config_tab(name, sound::id, false);

        config.add_percent_number_option(
            sound::options::main_volume,
            "Main Volume",
            "Controls the main volume of the game.",
            100.0
        );

        return config;
    }
} // namespace config
} // namespace recompui
