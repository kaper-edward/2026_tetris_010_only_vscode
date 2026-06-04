#include "SfmlRenderer.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include <utility>

using namespace tetris::render;

namespace {
constexpr float CELL = 28.0f;
constexpr float PREVIEW_CELL = 24.0f;
constexpr float OFFSET_X = 32.0f;
constexpr float OFFSET_Y = 32.0f;
constexpr float SIDE_X = OFFSET_X + RENDER_BOARD_WIDTH * CELL + 28.0f;
constexpr unsigned int PANEL_TEXT_SIZE = 20;

sf::Color board_cell_color(char value) {
    return value == '#' ? sf::Color(80, 220, 220) : sf::Color(35, 38, 42);
}
}

SfmlRenderer::SfmlRenderer(sf::RenderWindow& window, std::string font_path)
    : window_(window), font_path_(std::move(font_path)) {}

bool SfmlRenderer::initialize() {
    return font_.openFromFile(font_path_);
}

void SfmlRenderer::render(const RenderSnapshot& snapshot) {
    window_.clear(sf::Color(18, 20, 24));
    sf::RectangleShape cell({CELL - 1.0f, CELL - 1.0f});
    for (int y = 0; y < RENDER_BOARD_HEIGHT; ++y) {
        for (int x = 0; x < RENDER_BOARD_WIDTH; ++x) {
            cell.setPosition({OFFSET_X + x * CELL, OFFSET_Y + y * CELL});
            cell.setFillColor(board_cell_color(snapshot.board_rows[y][x]));
            window_.draw(cell);
        }
    }

    sf::Text text(font_);
    text.setString("Score: " + std::to_string(snapshot.score) +
                   "\nLevel: " + std::to_string(snapshot.level) +
                   "\nLines: " + std::to_string(snapshot.lines_cleared) +
                   (snapshot.status_text.empty() ? "" : "\n" + snapshot.status_text) +
                   (snapshot.running ? "" : "\nGame Over"));
    text.setCharacterSize(PANEL_TEXT_SIZE);
    text.setFillColor(sf::Color::White);
    text.setPosition({SIDE_X, OFFSET_Y});
    window_.draw(text);

    sf::Text next_label(font_);
    next_label.setString("Next");
    next_label.setCharacterSize(PANEL_TEXT_SIZE);
    next_label.setFillColor(sf::Color::White);
    next_label.setPosition({SIDE_X, OFFSET_Y + 110.0f});
    window_.draw(next_label);

    sf::RectangleShape preview_cell({PREVIEW_CELL - 1.0f, PREVIEW_CELL - 1.0f});
    for (int y = 0; y < RENDER_BLOCK_SIZE; ++y) {
        for (int x = 0; x < RENDER_BLOCK_SIZE; ++x) {
            preview_cell.setPosition({SIDE_X + x * PREVIEW_CELL, OFFSET_Y + 145.0f + y * PREVIEW_CELL});
            if (snapshot.next_piece.available && snapshot.next_piece.rows[y][x] == '#') {
                preview_cell.setFillColor(sf::Color(120, 210, 120));
            } else {
                preview_cell.setFillColor(sf::Color(30, 32, 36));
            }
            window_.draw(preview_cell);
        }
    }
    window_.display();
}
