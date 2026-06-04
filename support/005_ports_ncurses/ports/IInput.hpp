#ifndef TETRIS_ASSIGNMENT_005_IINPUT_HPP
#define TETRIS_ASSIGNMENT_005_IINPUT_HPP

#include "../../shared/core/GameAction.hpp"

#include <optional>

class IInput {
public:
    IInput() = default;
    IInput(const IInput&) = delete;
    IInput& operator=(const IInput&) = delete;
    virtual ~IInput() = default;

    // Called before pollAction() with the game-step tick about to be processed.
    virtual void advanceTick(int) {}
    virtual std::optional<GameAction> pollAction() = 0;
};

#endif
