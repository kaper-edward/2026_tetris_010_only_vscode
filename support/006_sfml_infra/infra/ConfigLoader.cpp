#include "ConfigLoader.hpp"

#include <charconv>
#include <fstream>
#include <string_view>

ConfigError::ConfigError(const std::string& message) : std::runtime_error(message) {}

namespace {

int parse_int(std::string_view value, std::string_view key) {
    int parsed = 0;
    const char* first = value.data();
    const char* last = value.data() + value.size();
    const auto result = std::from_chars(first, last, parsed);
    if (result.ec != std::errc{} || result.ptr != last) {
        throw ConfigError("invalid integer for " + std::string(key));
    }
    return parsed;
}

}  // namespace

GameConfig load_config_file(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        throw ConfigError("cannot open config: " + path);
    }

    GameConfig config;
    std::string line;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty() || line[0] == '#') {
            continue;
        }
        const std::size_t equal = line.find('=');
        if (equal == std::string::npos) {
            throw ConfigError("missing '=' in config line");
        }
        const std::string_view key(line.data(), equal);
        const std::string_view value(line.data() + equal + 1, line.size() - equal - 1);
        if (key == "board_width") {
            config.board_width = parse_int(value, key);
        } else if (key == "board_height") {
            config.board_height = parse_int(value, key);
        } else if (key == "tick_rate") {
            config.tick_rate = parse_int(value, key);
        } else if (key == "font_path") {
            config.font_path = std::string(value);
        } else {
            throw ConfigError("unknown config key: " + std::string(key));
        }
    }
    if (config.board_width <= 0 || config.board_height <= 0 || config.tick_rate <= 0) {
        throw ConfigError("config dimensions and tick_rate must be positive");
    }
    return config;
}
