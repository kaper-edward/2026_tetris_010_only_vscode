#ifndef TETRIS_ASSIGNMENT_006_SFML_RENDERER_HPP
#define TETRIS_ASSIGNMENT_006_SFML_RENDERER_HPP

#include "IRenderer.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <string>

class SfmlRenderer final : public IRenderer {
public:
    explicit SfmlRenderer(sf::RenderWindow& window, std::string font_path);

    bool initialize();
    void render(const tetris::render::RenderSnapshot& snapshot) override;

private:
    sf::RenderWindow& window_;
    std::string font_path_;
    sf::Font font_;
};

#endif
