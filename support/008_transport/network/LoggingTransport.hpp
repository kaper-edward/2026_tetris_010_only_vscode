#ifndef TETRIS_ASSIGNMENT_008_LOGGING_TRANSPORT_HPP
#define TETRIS_ASSIGNMENT_008_LOGGING_TRANSPORT_HPP

#include "ITransport.hpp"

#include <iosfwd>
#include <memory>
#include <optional>
#include <string>

namespace tetris::net {

class LoggingTransport final : public ITransport {
public:
    LoggingTransport(std::unique_ptr<ITransport> inner, std::ostream& log);

    void send(std::string line) override;
    std::optional<std::string> receive() override;
    bool disconnected() const noexcept override;

private:
    std::unique_ptr<ITransport> inner_;
    std::ostream& log_;
};

}  // namespace tetris::net

#endif
