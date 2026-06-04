#ifndef TETRIS_ASSIGNMENT_005_NCURSES_INPUT_HPP
#define TETRIS_ASSIGNMENT_005_NCURSES_INPUT_HPP

#include "IInput.hpp"

class NcursesInput final : public IInput {
public:
    std::optional<GameAction> pollAction() override;
};

#endif
