#ifndef TETRIS_ASSIGNMENT_005_RECORDING_INPUT_HPP
#define TETRIS_ASSIGNMENT_005_RECORDING_INPUT_HPP

#include "IInput.hpp"
#include "tetris.h"

#include <optional>
#include <ostream>
#include <utility>
#include <vector>

struct InputLogEntry {
    int tick = 0;
    GameAction action = GameAction::None;
};

class RecordingInput final : public IInput {
public:
    explicit RecordingInput(IInput& inner) : inner_(inner) {}

    void advanceTick(int tick) override {
        current_tick_ = tick;
    }

    std::optional<GameAction> pollAction() override {
        auto action = inner_.pollAction();
        if (action) {
            entries_.push_back({current_tick_, *action});
        }
        return action;
    }

    const std::vector<InputLogEntry>& entries() const {
        return entries_;
    }

    void save(std::ostream& out) const {
        out << "tick action\n";
        for (const auto& entry : entries_) {
            out << entry.tick << ' ' << action_to_string(entry.action) << '\n';
        }
    }

private:
    IInput& inner_;
    int current_tick_ = 0;
    std::vector<InputLogEntry> entries_;
};

#endif
