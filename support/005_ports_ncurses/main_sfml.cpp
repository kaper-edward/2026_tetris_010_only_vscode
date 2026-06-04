#include "LocalGameApp.hpp"
#include "SfmlInput.hpp"
#include "SfmlRenderer.hpp"

#include <SFML/Graphics.hpp>

namespace {

constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 700;
constexpr int TICK_RATE = 20;

}  // namespace

int main() {
    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "2026 Tetris v2 - Assignment 005");
    window.setFramerateLimit(60);

    SfmlRenderer renderer(window, "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    if (!renderer.initialize()) {
        return 2;
    }
    SfmlInput input(window);
    LocalGameApp app(GameCore(888), renderer, input, TICK_RATE);
    while (window.isOpen()) {
        app.singleTick();
        sf::sleep(sf::milliseconds(1000 / TICK_RATE));
    }
    return 0;
}
