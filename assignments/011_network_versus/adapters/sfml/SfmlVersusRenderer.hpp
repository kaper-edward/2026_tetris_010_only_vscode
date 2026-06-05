#ifndef TETRIS_ASSIGNMENT_011_SFML_VERSUS_RENDERER_HPP
#define TETRIS_ASSIGNMENT_011_SFML_VERSUS_RENDERER_HPP

#include "VersusViewSnapshot.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <string>

class SfmlVersusRenderer {
public:
    SfmlVersusRenderer(sf::RenderWindow& window, std::string font_path);

    bool initialize();
    void render(const tetris::view::VersusViewSnapshot& snapshot);

private:
    sf::RenderWindow& window_;
    std::string font_path_;
    sf::Font font_;
};

#endif
