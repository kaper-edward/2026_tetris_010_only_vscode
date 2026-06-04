#include "ConfigLoader.hpp"
#include "LocalGameApp.hpp"
#include "SfmlInput.hpp"
#include "SfmlRenderer.hpp"

#include <SFML/Graphics.hpp>

int main() {
    GameConfig config;
    sf::RenderWindow window(sf::VideoMode({640, 700}), "2026 Tetris v2");
    window.setFramerateLimit(60);

    SfmlRenderer renderer(window, config.font_path);
    if (!renderer.initialize()) {
        return 2;
    }
    SfmlInput input(window);
    LocalGameApp app(GameCore(888), renderer, input, config.tick_rate);
    while (window.isOpen()) {
        app.singleTick();
        sf::sleep(sf::milliseconds(1000 / config.tick_rate));
    }
    return 0;
}
