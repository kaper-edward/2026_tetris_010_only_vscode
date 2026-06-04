#include "SfmlInput.hpp"

#include <SFML/Window/Event.hpp>

SfmlInput::SfmlInput(sf::RenderWindow& window) : window_(window) {}

std::optional<GameAction> SfmlInput::pollAction() {
    while (const auto event = window_.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window_.close();
            return GameAction::Quit;
        }
        if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            switch (key->code) {
                case sf::Keyboard::Key::Left:
                    return GameAction::Left;
                case sf::Keyboard::Key::Right:
                    return GameAction::Right;
                case sf::Keyboard::Key::Down:
                    return GameAction::Down;
                case sf::Keyboard::Key::Up:
                    return GameAction::Rotate;
                case sf::Keyboard::Key::Space:
                    return GameAction::Drop;
                case sf::Keyboard::Key::Escape:
                case sf::Keyboard::Key::Q:
                    return GameAction::Quit;
                default:
                    break;
            }
        }
    }
    return std::nullopt;
}
