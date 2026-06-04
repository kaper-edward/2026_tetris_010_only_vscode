#include "InMemoryTransport.hpp"

#include <utility>

namespace tetris::net {

void InMemoryTransport::send(std::string line) {
    sent_.push_back(std::move(line));
}

std::optional<std::string> InMemoryTransport::receive() {
    if (incoming_.empty()) {
        return std::nullopt;
    }
    std::string line = std::move(incoming_.front());
    incoming_.pop();
    return line;
}

bool InMemoryTransport::disconnected() const noexcept {
    return disconnected_;
}

void InMemoryTransport::pushIncoming(std::string line) {
    incoming_.push(std::move(line));
}

void InMemoryTransport::disconnect() {
    disconnected_ = true;
}

const std::vector<std::string>& InMemoryTransport::sentLines() const {
    return sent_;
}

}  // namespace tetris::net
