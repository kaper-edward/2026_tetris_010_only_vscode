#ifndef TETRIS_ASSIGNMENT_006_CONFIG_LOADER_HPP
#define TETRIS_ASSIGNMENT_006_CONFIG_LOADER_HPP

#include <stdexcept>
#include <string>

struct GameConfig {
    int board_width = 10;
    int board_height = 20;
    int tick_rate = 20;
    std::string font_path = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
};

class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& message);
};

GameConfig load_config_file(const std::string& path);

#endif
