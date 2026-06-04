#ifndef TETRIS_ASSIGNMENT_008_IN_MEMORY_TRANSPORT_HPP
#define TETRIS_ASSIGNMENT_008_IN_MEMORY_TRANSPORT_HPP

#include "ITransport.hpp"

#include <queue>
#include <string>
#include <vector>

namespace tetris::net {

class InMemoryTransport final : public ITransport {
public:
    void send(std::string line) override;
    std::optional<std::string> receive() override;
    bool disconnected() const noexcept override;

    void pushIncoming(std::string line);
    void disconnect();
    const std::vector<std::string>& sentLines() const;

private:
    std::queue<std::string> incoming_;
    std::vector<std::string> sent_;
    bool disconnected_ = false;
};

}  // namespace tetris::net

#endif
