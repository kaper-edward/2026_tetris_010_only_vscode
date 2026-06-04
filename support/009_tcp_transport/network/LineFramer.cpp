#include "LineFramer.hpp"

namespace tetris::net {

bool LineFramer::pushBytes(std::string_view data) {
    if (failed_) {
        return false;
    }
    buffer_.append(data.data(), data.size());

    // Re-scan retained buffered lines; already-valid complete lines stay valid.
    std::size_t start = 0;
    while (true) {
        const std::size_t newline = buffer_.find('\n', start);
        if (newline == std::string::npos) {
            break;
        }
        std::size_t line_size = newline - start;
        if (line_size > 0) {
            const std::size_t last_content = newline - 1;
            if (buffer_[last_content] == '\r') {
                --line_size;
            }
        }
        if (line_size > MAX_LINE_BYTES) {
            failed_ = true;
            return false;
        }
        start = newline + 1;
    }

    if (buffer_.size() - start > MAX_LINE_BYTES) {
        failed_ = true;
        return false;
    }
    return true;
}

std::optional<std::string> LineFramer::popLine() {
    if (failed_) {
        return std::nullopt;
    }
    const std::size_t newline = buffer_.find('\n');
    if (newline == std::string::npos) {
        return std::nullopt;
    }
    std::string line = buffer_.substr(0, newline);
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    buffer_.erase(0, newline + 1);
    return line;
}

bool LineFramer::failed() const noexcept {
    return failed_;
}

std::string frame_line(std::string line) {
    while (!line.empty() && (line.back() == '\n' || line.back() == '\r')) {
        line.pop_back();
    }
    line.push_back('\n');
    return line;
}

}  // namespace tetris::net
