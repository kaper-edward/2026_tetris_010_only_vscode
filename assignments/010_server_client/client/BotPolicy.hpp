#ifndef TETRIS_ASSIGNMENT_010_BOT_POLICY_HPP
#define TETRIS_ASSIGNMENT_010_BOT_POLICY_HPP

#include "GameAction.hpp"
#include "RemoteSnapshot.hpp"

#include <optional>

class IBotPolicy {
public:
    virtual ~IBotPolicy() = default;
    virtual std::optional<tetris::protocol::GameAction> decide(int snapshot_count, tetris::protocol::PlayerId player_id) = 0;
};

class DropOncePolicy final : public IBotPolicy {
public:
    std::optional<tetris::protocol::GameAction> decide(int, tetris::protocol::PlayerId) override {
        if (fired_) {
            return std::nullopt;
        }
        fired_ = true;
        return tetris::protocol::GameAction::Drop;
    }

private:
    bool fired_ = false;
};

class SidePolicy final : public IBotPolicy {
public:
    explicit SidePolicy(int interval) : interval_(interval) {}

    std::optional<tetris::protocol::GameAction> decide(int snapshot_count, tetris::protocol::PlayerId player_id) override {
        if (snapshot_count < last_snapshot_ + interval_) {
            return std::nullopt;
        }
        last_snapshot_ = snapshot_count;
        return player_id == 1 ? tetris::protocol::GameAction::Left : tetris::protocol::GameAction::Right;
    }

private:
    int interval_ = 1;
    int last_snapshot_ = 0;
};

#endif
