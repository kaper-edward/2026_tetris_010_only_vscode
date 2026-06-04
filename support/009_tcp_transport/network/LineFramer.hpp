#ifndef TETRIS_ASSIGNMENT_009_LINE_FRAMER_HPP
#define TETRIS_ASSIGNMENT_009_LINE_FRAMER_HPP

#include <optional>
#include <string>
#include <string_view>

namespace tetris::net {

constexpr std::size_t MAX_LINE_BYTES = 8192;

class LineFramer {
public:
    bool pushBytes(std::string_view data);
    std::optional<std::string> popLine();
    bool failed() const noexcept;

private:
    std::string buffer_;
    bool failed_ = false;
};

std::string frame_line(std::string line);

}  // namespace tetris::net

#endif
