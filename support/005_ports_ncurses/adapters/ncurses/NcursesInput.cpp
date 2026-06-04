#include "NcursesInput.hpp"

#include <ncurses.h>

std::optional<GameAction> NcursesInput::pollAction() {
    const int ch = getch();
    switch (ch) {
        case KEY_LEFT:
            return GameAction::Left;
        case KEY_RIGHT:
            return GameAction::Right;
        case KEY_DOWN:
            return GameAction::Down;
        case KEY_UP:
            return GameAction::Rotate;
        case ' ':
            return GameAction::Drop;
        case 'q':
        case 'Q':
            return GameAction::Quit;
        default:
            return std::nullopt;
    }
}
