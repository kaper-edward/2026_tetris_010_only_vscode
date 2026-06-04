#ifndef TETRIS_ASSIGNMENT_005_REPLAY_INPUT_HPP
#define TETRIS_ASSIGNMENT_005_REPLAY_INPUT_HPP

#include "RecordingInput.hpp"

#include <istream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

class ReplayInput final : public IInput {
public:
    ReplayInput() = default;
    explicit ReplayInput(std::istream& in) {
        load(in);
    }

    void advanceTick(int tick) override {
        current_tick_ = tick;
    }

    std::optional<GameAction> pollAction() override {
        if (index_ >= entries_.size() || entries_[index_].tick != current_tick_) {
            return std::nullopt;
        }
        return entries_[index_++].action;
    }

    const std::vector<InputLogEntry>& entries() const {
        return entries_;
    }

private:
    void load(std::istream& in) {
        std::string first;
        std::string second;
        if (!(in >> first >> second)) {
            return;
        }
        if (first != "tick" || second != "action") {
            int first_tick = 0;
            std::istringstream tick_stream(first);
            if (!(tick_stream >> first_tick)) {
                return;
            }
            entries_.push_back({first_tick, parse_action(second)});
        }
        int tick = 0;
        std::string token;
        while (in >> tick >> token) {
            entries_.push_back({tick, parse_action(token)});
        }
    }

    int current_tick_ = 0;
    std::size_t index_ = 0;
    std::vector<InputLogEntry> entries_;
};

#endif
