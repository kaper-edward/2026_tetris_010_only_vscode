#ifndef TETRIS_ASSIGNMENT_009_ITRANSPORT_HPP
#define TETRIS_ASSIGNMENT_009_ITRANSPORT_HPP

#include <optional>
#include <string>

namespace tetris::net {

class ITransport {
public:
    ITransport() = default;
    ITransport(const ITransport&) = delete;
    ITransport& operator=(const ITransport&) = delete;
    virtual ~ITransport() = default;

    virtual void send(std::string line) = 0;
    virtual std::optional<std::string> receive() = 0;
    virtual bool disconnected() const noexcept = 0;
};

}  // namespace tetris::net

#endif
