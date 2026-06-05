#include "SfmlVersusRenderer.hpp"

#include "TetrominoPreview.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>

using namespace tetris::render;
using namespace tetris::view;

namespace {

constexpr float CELL = 30.0f;
constexpr float PREVIEW_CELL = 26.0f;
constexpr unsigned int SMALL_TEXT = 20;
constexpr unsigned int TOP5_ROW_TEXT = 10;
constexpr unsigned int BODY_TEXT = 26;
constexpr unsigned int TITLE_TEXT = 34;
constexpr unsigned int VS_TEXT = 64;
constexpr std::size_t TOP5_NAME_CHARS = 7;

struct Rect {
    float x;
    float y;
    float w;
    float h;
};

struct PlayerLayout {
    Rect score;
    Rect board;
    Rect stats;
    sf::Color accent;
    std::string label;
};

const PlayerLayout P1_LAYOUT{
    {130.0f, 28.0f, 360.0f, 92.0f},
    {160.0f, 178.0f, RENDER_BOARD_WIDTH * CELL, RENDER_BOARD_HEIGHT * CELL},
    {100.0f, 794.0f, 420.0f, 64.0f},
    sf::Color(220, 36, 34),
    "PLAYER 1",
};

const PlayerLayout P2_LAYOUT{
    {1110.0f, 28.0f, 360.0f, 92.0f},
    {1140.0f, 178.0f, RENDER_BOARD_WIDTH * CELL, RENDER_BOARD_HEIGHT * CELL},
    {1080.0f, 794.0f, 420.0f, 64.0f},
    sf::Color(28, 92, 220),
    "PLAYER 2",
};

constexpr Rect HIGH_SCORE_PANEL{620.0f, 28.0f, 360.0f, 92.0f};
constexpr Rect CENTER_COLUMN{640.0f, 132.0f, 320.0f, 672.0f};

const std::array<sf::Color, 7> BLOCK_COLORS{{
    sf::Color(0, 222, 232),
    sf::Color(255, 215, 24),
    sf::Color(48, 225, 42),
    sf::Color(45, 100, 255),
    sf::Color(155, 50, 255),
    sf::Color(255, 130, 20),
    sf::Color(238, 40, 46),
}};

std::size_t to_index(int value) {
    assert(value >= 0);
    return static_cast<std::size_t>(value);
}

char board_cell(const RenderSnapshot& snapshot, int row, int col) {
    if (row < 0 || row >= RENDER_BOARD_HEIGHT || col < 0 || col >= RENDER_BOARD_WIDTH) {
        return '.';
    }
    const std::string& cells = snapshot.board_rows[to_index(row)];
    if (static_cast<int>(cells.size()) <= col) {
        return '.';
    }
    return cells[to_index(col)];
}

char color_cell(const RenderSnapshot& snapshot, int row, int col) {
    if (!snapshot.has_color_board || row < 0 || row >= RENDER_BOARD_HEIGHT || col < 0 || col >= RENDER_BOARD_WIDTH) {
        return '.';
    }
    const std::string& cells = snapshot.color_board_rows[to_index(row)];
    if (static_cast<int>(cells.size()) <= col) {
        return '.';
    }
    return cells[to_index(col)];
}

char preview_cell(const PiecePreview& preview, int row, int col) {
    if (!preview.available || row < 0 || row >= RENDER_BLOCK_SIZE || col < 0 || col >= RENDER_BLOCK_SIZE) {
        return '.';
    }
    const std::string& cells = preview.rows[to_index(row)];
    if (static_cast<int>(cells.size()) <= col) {
        return '.';
    }
    return cells[to_index(col)];
}

char piece_symbol(int tetromino) {
    static constexpr std::array<char, 7> symbols{{'I', 'O', 'T', 'S', 'Z', 'J', 'L'}};
    if (tetromino < 0 || tetromino >= static_cast<int>(symbols.size())) {
        return '#';
    }
    return symbols[static_cast<std::size_t>(tetromino)];
}

std::string format_score(int score) {
    std::ostringstream out;
    out << std::setw(7) << std::setfill('0') << std::max(0, score);
    return out.str();
}

std::string format_time(std::uint32_t tick, int tick_rate) {
    const std::uint32_t seconds = tick / static_cast<std::uint32_t>(std::max(1, tick_rate));
    std::ostringstream out;
    out << std::setw(2) << std::setfill('0') << seconds / 60 << ":"
        << std::setw(2) << std::setfill('0') << seconds % 60;
    return out.str();
}

std::string format_top5_name(std::string name) {
    if (name.size() > TOP5_NAME_CHARS) {
        name.resize(TOP5_NAME_CHARS);
    }
    return name;
}

void draw_rect(sf::RenderWindow& window, Rect rect, sf::Color fill) {
    sf::RectangleShape shape({rect.w, rect.h});
    shape.setPosition({rect.x, rect.y});
    shape.setFillColor(fill);
    window.draw(shape);
}

void draw_text(sf::RenderWindow& window,
               const sf::Font& font,
               const std::string& text,
               float x,
               float y,
               unsigned int size,
               sf::Color color = sf::Color::White,
               bool centered = false) {
    sf::Text label(font);
    label.setString(text);
    label.setCharacterSize(size);
    label.setFillColor(color);
    label.setStyle(sf::Text::Bold);
    if (centered) {
        const sf::FloatRect bounds = label.getLocalBounds();
        label.setOrigin({bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + bounds.size.y / 2.0f});
    }
    label.setPosition({x, y});
    window.draw(label);
}

void draw_text_right(sf::RenderWindow& window,
                     const sf::Font& font,
                     const std::string& text,
                     float x,
                     float y,
                     unsigned int size,
                     sf::Color color = sf::Color::White) {
    sf::Text label(font);
    label.setString(text);
    label.setCharacterSize(size);
    label.setFillColor(color);
    label.setStyle(sf::Text::Bold);
    const sf::FloatRect bounds = label.getLocalBounds();
    label.setPosition({x - (bounds.position.x + bounds.size.x), y});
    window.draw(label);
}

void draw_panel(sf::RenderWindow& window, Rect rect, sf::Color border = sf::Color(210, 216, 226)) {
    draw_rect(window, rect, sf::Color(6, 8, 12));

    sf::RectangleShape outer({rect.w, rect.h});
    outer.setPosition({rect.x, rect.y});
    outer.setFillColor(sf::Color::Transparent);
    outer.setOutlineThickness(4.0f);
    outer.setOutlineColor(border);
    window.draw(outer);

    sf::RectangleShape inner({rect.w - 14.0f, rect.h - 14.0f});
    inner.setPosition({rect.x + 7.0f, rect.y + 7.0f});
    inner.setFillColor(sf::Color::Transparent);
    inner.setOutlineThickness(2.0f);
    inner.setOutlineColor(sf::Color(78, 86, 102));
    window.draw(inner);
}

void draw_score_panel(sf::RenderWindow& window,
                      const sf::Font& font,
                      Rect rect,
                      const std::string& label,
                      const std::string& value,
                      sf::Color label_color,
                      sf::Color value_color = sf::Color::White) {
    draw_panel(window, rect);
    draw_text(window, font, label, rect.x + rect.w / 2.0f, rect.y + 24.0f, BODY_TEXT, label_color, true);
    draw_text(window, font, value, rect.x + rect.w / 2.0f, rect.y + 63.0f, TITLE_TEXT, value_color, true);
}

void draw_header_bar(sf::RenderWindow& window, const sf::Font& font, Rect board, const PlayerLayout& layout) {
    Rect header{board.x, board.y - 44.0f, board.w, 40.0f};
    draw_rect(window, header, layout.accent);
    sf::RectangleShape border({header.w, header.h});
    border.setPosition({header.x, header.y});
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineThickness(3.0f);
    border.setOutlineColor(sf::Color(210, 216, 226));
    window.draw(border);
    draw_text(window, font, layout.label, header.x + header.w / 2.0f, header.y + 20.0f, BODY_TEXT, sf::Color::White, true);
}

sf::Color fallback_block_color(int col, ViewPlayerId player) {
    const int index = (col + std::max(0, player)) % static_cast<int>(BLOCK_COLORS.size());
    return BLOCK_COLORS[static_cast<std::size_t>(index)];
}

sf::Color block_color(char symbol, int col, ViewPlayerId player) {
    switch (symbol) {
        case 'I': return sf::Color(0, 222, 232);
        case 'O': return sf::Color(255, 215, 24);
        case 'T': return sf::Color(155, 50, 255);
        case 'S': return sf::Color(48, 225, 42);
        case 'Z': return sf::Color(238, 40, 46);
        case 'J': return sf::Color(45, 100, 255);
        case 'L': return sf::Color(255, 130, 20);
        case 'G': return sf::Color(145, 154, 170);
        default: return fallback_block_color(col, player);
    }
}

void draw_block(sf::RenderWindow& window, Rect cell, sf::Color color) {
    draw_rect(window, cell, color);
    draw_rect(window, {cell.x, cell.y, cell.w, 3.0f}, sf::Color(255, 255, 255, 90));
    draw_rect(window, {cell.x, cell.y, 3.0f, cell.h}, sf::Color(255, 255, 255, 80));
    draw_rect(window, {cell.x, cell.y + cell.h - 4.0f, cell.w, 4.0f}, sf::Color(0, 0, 0, 80));
    draw_rect(window, {cell.x + cell.w - 4.0f, cell.y, 4.0f, cell.h}, sf::Color(0, 0, 0, 70));
}

void draw_ghost_cell(sf::RenderWindow& window, Rect cell, sf::Color color) {
    sf::RectangleShape outline({cell.w, cell.h});
    outline.setPosition({cell.x, cell.y});
    outline.setFillColor(sf::Color::Transparent);
    outline.setOutlineThickness(2.0f);
    outline.setOutlineColor(sf::Color(color.r, color.g, color.b, 180));
    window.draw(outline);
}

void draw_board_body(sf::RenderWindow& window, const RenderSnapshot& snapshot, Rect board, ViewPlayerId player) {
    draw_panel(window, {board.x - 10.0f, board.y - 10.0f, board.w + 20.0f, board.h + 20.0f});
    draw_rect(window, board, sf::Color(2, 3, 6));

    sf::RectangleShape grid_line;
    grid_line.setFillColor(sf::Color(44, 55, 72));
    for (int col = 0; col <= RENDER_BOARD_WIDTH; ++col) {
        grid_line.setSize({1.0f, board.h});
        grid_line.setPosition({board.x + col * CELL, board.y});
        window.draw(grid_line);
    }
    for (int row = 0; row <= RENDER_BOARD_HEIGHT; ++row) {
        grid_line.setSize({board.w, 1.0f});
        grid_line.setPosition({board.x, board.y + row * CELL});
        window.draw(grid_line);
    }

    if (snapshot.has_ghost_y && snapshot.ghost_y != snapshot.block_y) {
        const char symbol = piece_symbol(snapshot.current_tetromino);
        const sf::Color color = block_color(symbol, snapshot.block_x, player);
        for (const auto& [col_offset, row_offset] : tetris::render::tetromino_cells(snapshot.current_tetromino, snapshot.current_rotation)) {
            const int col = snapshot.block_x + col_offset;
            const int row = snapshot.ghost_y + row_offset;
            if (row < 0 || row >= RENDER_BOARD_HEIGHT || col < 0 || col >= RENDER_BOARD_WIDTH || board_cell(snapshot, row, col) == '#') {
                continue;
            }
            const Rect cell{
                board.x + col * CELL + 5.0f,
                board.y + row * CELL + 5.0f,
                CELL - 10.0f,
                CELL - 10.0f,
            };
            draw_ghost_cell(window, cell, color);
        }
    }

    for (int row = 0; row < RENDER_BOARD_HEIGHT; ++row) {
        for (int col = 0; col < RENDER_BOARD_WIDTH; ++col) {
            if (board_cell(snapshot, row, col) != '#') {
                continue;
            }
            const Rect cell{
                board.x + col * CELL + 2.0f,
                board.y + row * CELL + 2.0f,
                CELL - 4.0f,
                CELL - 4.0f,
            };
            draw_block(window, cell, block_color(color_cell(snapshot, row, col), col, player));
        }
    }
}

void draw_player_area(sf::RenderWindow& window, const sf::Font& font, const PlayerViewSnapshot& player, const PlayerLayout& layout) {
    draw_header_bar(window, font, layout.board, layout);
    draw_board_body(window, player.board, layout.board, player.player);

    draw_panel(window, layout.stats);
    draw_text(window, font, "LEVEL", layout.stats.x + 78.0f, layout.stats.y + 22.0f, SMALL_TEXT, sf::Color::White, true);
    draw_text(window, font, std::to_string(player.board.level), layout.stats.x + 78.0f, layout.stats.y + 49.0f, BODY_TEXT, sf::Color(255, 222, 34), true);
    draw_text(window, font, "|", layout.stats.x + layout.stats.w / 2.0f, layout.stats.y + 36.0f, BODY_TEXT, sf::Color(210, 216, 226), true);
    draw_text(window, font, "LINES", layout.stats.x + layout.stats.w - 78.0f, layout.stats.y + 22.0f, SMALL_TEXT, sf::Color::White, true);
    draw_text(window, font, std::to_string(player.board.lines_cleared), layout.stats.x + layout.stats.w - 78.0f, layout.stats.y + 49.0f, BODY_TEXT, sf::Color(66, 235, 66), true);
}

void draw_next_panel(sf::RenderWindow& window, const sf::Font& font, const RenderSnapshot& snapshot, Rect rect, ViewPlayerId player) {
    draw_panel(window, rect);
    draw_text(window, font, "NEXT", rect.x + rect.w / 2.0f, rect.y + 25.0f, BODY_TEXT, sf::Color::White, true);
    const float origin_x = rect.x + rect.w / 2.0f - PREVIEW_CELL * 2.0f;
    const float origin_y = rect.y + 52.0f;
    for (int row = 0; row < RENDER_BLOCK_SIZE; ++row) {
        for (int col = 0; col < RENDER_BLOCK_SIZE; ++col) {
            if (preview_cell(snapshot.next_piece, row, col) != '#') {
                continue;
            }
            const Rect cell{origin_x + col * PREVIEW_CELL, origin_y + row * PREVIEW_CELL, PREVIEW_CELL - 3.0f, PREVIEW_CELL - 3.0f};
            draw_block(window, cell, block_color(piece_symbol(snapshot.next_piece.tetromino), col, player));
        }
    }
}

void draw_top5_panel(sf::RenderWindow& window, const sf::Font& font, Rect rect, const VersusViewSnapshot& snapshot) {
    draw_panel(window, rect);
    draw_text(window, font, "TOP 5", rect.x + rect.w / 2.0f, rect.y + 27.0f, BODY_TEXT, sf::Color(255, 222, 34), true);

    const std::size_t count = std::min<std::size_t>(5, snapshot.top_scores.size());
    for (std::size_t i = 0; i < count; ++i) {
        const float row_y = rect.y + 66.0f + static_cast<float>(i) * 24.0f;
        draw_text(window, font, std::to_string(i + 1) + ".", rect.x + 24.0f, row_y, TOP5_ROW_TEXT, sf::Color(210, 216, 226));
        draw_text(window,
                  font,
                  format_top5_name(snapshot.top_scores[i].name),
                  rect.x + 54.0f,
                  row_y,
                  TOP5_ROW_TEXT,
                  sf::Color(0, 220, 232));
        draw_text_right(window,
                        font,
                        format_score(snapshot.top_scores[i].score),
                        rect.x + rect.w - 26.0f,
                        row_y,
                        TOP5_ROW_TEXT,
                        sf::Color(0, 220, 232));
    }
}

void draw_center_column(sf::RenderWindow& window,
                        const sf::Font& font,
                        const VersusViewSnapshot& snapshot,
                        const PlayerViewSnapshot& p1,
                        const PlayerViewSnapshot& p2) {
    draw_text(window, font, "VS", CENTER_COLUMN.x + CENTER_COLUMN.w / 2.0f, CENTER_COLUMN.y + 48.0f, VS_TEXT, sf::Color(255, 218, 28), true);
    draw_next_panel(window, font, p1.board, {705.0f, 220.0f, 190.0f, 118.0f}, p1.player);
    draw_next_panel(window, font, p2.board, {705.0f, 362.0f, 190.0f, 118.0f}, p2.player);

    draw_score_panel(window, font, {705.0f, 504.0f, 190.0f, 86.0f}, "TIME", format_time(snapshot.tick, snapshot.tick_rate), sf::Color::White, sf::Color(0, 220, 232));
    draw_top5_panel(window, font, {665.0f, 612.0f, 270.0f, 202.0f}, snapshot);

    const std::string status = snapshot.status_text.empty() ? versus_phase_name(snapshot.phase) : snapshot.status_text;
    if (snapshot.phase != VersusViewPhase::Playing) {
        draw_text(window, font, status, CENTER_COLUMN.x + CENTER_COLUMN.w / 2.0f, 136.0f, SMALL_TEXT, sf::Color(210, 216, 226), true);
    }
    if (snapshot.winner != 0) {
        draw_text(window, font, "WINNER P" + std::to_string(snapshot.winner), CENTER_COLUMN.x + CENTER_COLUMN.w / 2.0f, 848.0f, BODY_TEXT, sf::Color(255, 80, 80), true);
    } else {
        draw_text(window, font, "INSERT COIN", CENTER_COLUMN.x + CENTER_COLUMN.w / 2.0f, 866.0f, BODY_TEXT, sf::Color(255, 222, 34), true);
    }
}

void draw_background(sf::RenderWindow& window) {
    window.clear(sf::Color(6, 8, 13));
    draw_rect(window, {0.0f, 0.0f, 1600.0f, 900.0f}, sf::Color(7, 10, 18));
}

const PlayerViewSnapshot& player_one(const VersusViewSnapshot& snapshot) {
    return snapshot.opponent.player == 1 ? snapshot.opponent : snapshot.you;
}

const PlayerViewSnapshot& player_two(const VersusViewSnapshot& snapshot) {
    return snapshot.you.player == 2 ? snapshot.you : snapshot.opponent;
}

}  // namespace

SfmlVersusRenderer::SfmlVersusRenderer(sf::RenderWindow& window, std::string font_path)
    : window_(window), font_path_(std::move(font_path)) {}

bool SfmlVersusRenderer::initialize() {
    return font_.openFromFile(font_path_);
}

void SfmlVersusRenderer::render(const VersusViewSnapshot& snapshot) {
    draw_background(window_);

    const PlayerViewSnapshot& p1 = player_one(snapshot);
    const PlayerViewSnapshot& p2 = player_two(snapshot);

    const int high_score = snapshot.has_high_score ? snapshot.high_score : std::max(p1.board.score, p2.board.score);
    draw_score_panel(window_, font_, P1_LAYOUT.score, "P1 SCORE", format_score(p1.board.score), sf::Color(255, 82, 32));
    draw_score_panel(window_, font_, HIGH_SCORE_PANEL, "HIGH SCORE", format_score(high_score), sf::Color(255, 222, 34));
    draw_score_panel(window_, font_, P2_LAYOUT.score, "P2 SCORE", format_score(p2.board.score), sf::Color(0, 220, 232));

    draw_player_area(window_, font_, p1, P1_LAYOUT);
    draw_player_area(window_, font_, p2, P2_LAYOUT);
    draw_center_column(window_, font_, snapshot, p1, p2);

    if (snapshot.phase == VersusViewPhase::GameOver) {
        const std::string reason = snapshot.reason.empty() ? "GAME OVER" : "GAME OVER " + snapshot.reason;
        draw_text(window_, font_, reason, 800.0f, 880.0f, SMALL_TEXT, sf::Color(255, 80, 80), true);
    }

    window_.display();
}
