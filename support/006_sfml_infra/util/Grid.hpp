#ifndef TETRIS_ASSIGNMENT_006_GRID_HPP
#define TETRIS_ASSIGNMENT_006_GRID_HPP

#include <array>
#include <cstddef>
#include <stdexcept>

template <typename T, std::size_t Width, std::size_t Height>
class Grid {
public:
    Grid() = default;

    constexpr std::size_t width() const noexcept {
        return Width;
    }

    constexpr std::size_t height() const noexcept {
        return Height;
    }

    const T& at(std::size_t x, std::size_t y) const {
        if (x >= Width || y >= Height) {
            throw std::out_of_range("Grid::at");
        }
        return cells_[y][x];
    }

    T& at(std::size_t x, std::size_t y) {
        if (x >= Width || y >= Height) {
            throw std::out_of_range("Grid::at");
        }
        return cells_[y][x];
    }

    const T& at(int x, int y) const {
        if (x < 0 || y < 0) {
            throw std::out_of_range("Grid::at");
        }
        return at(static_cast<std::size_t>(x), static_cast<std::size_t>(y));
    }

    T& at(int x, int y) {
        if (x < 0 || y < 0) {
            throw std::out_of_range("Grid::at");
        }
        return at(static_cast<std::size_t>(x), static_cast<std::size_t>(y));
    }

    void fill(const T& value) {
        for (auto& row : cells_) {
            row.fill(value);
        }
    }

private:
    std::array<std::array<T, Width>, Height> cells_{};
};

#endif
