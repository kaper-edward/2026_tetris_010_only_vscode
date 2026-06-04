#ifndef TETRIS_ASSIGNMENT_005_SFML_INPUT_HPP
#define TETRIS_ASSIGNMENT_005_SFML_INPUT_HPP

#include "IInput.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

class SfmlInput final : public IInput {
public:
    explicit SfmlInput(sf::RenderWindow& window);

    std::optional<GameAction> pollAction() override;

private:
    sf::RenderWindow& window_;
};

#endif
