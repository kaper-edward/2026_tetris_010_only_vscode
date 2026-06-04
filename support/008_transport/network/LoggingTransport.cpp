#include "LoggingTransport.hpp"

#include <cassert>
#include <ostream>
#include <utility>

namespace tetris::net {

LoggingTransport::LoggingTransport(std::unique_ptr<ITransport> inner, std::ostream& log)
    : inner_(std::move(inner)), log_(log) {
    assert(inner_ != nullptr);
}

void LoggingTransport::send(std::string line) {
    log_ << "C " << line << '\n';
    inner_->send(std::move(line));
}

std::optional<std::string> LoggingTransport::receive() {
    auto line = inner_->receive();
    if (line) {
        log_ << "S " << *line << '\n';
    }
    return line;
}

bool LoggingTransport::disconnected() const noexcept {
    return inner_->disconnected();
}

}  // namespace tetris::net
