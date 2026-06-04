#include "ScoreRepository.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <system_error>
#include <utility>

ScoreError::ScoreError(const std::string& message) : std::runtime_error(message) {}

ScoreRepository::ScoreRepository(std::string path) : path_(std::move(path)) {}

std::vector<int> ScoreRepository::load() const {
    std::ifstream input(path_);
    if (!input) {
        std::error_code error;
        const bool exists = std::filesystem::exists(path_, error);
        if (!error && !exists) {
            return {};
        }
        throw ScoreError("cannot open score file");
    }
    std::vector<int> scores;
    int score = 0;
    while (input >> score) {
        scores.push_back(score);
    }
    if (!input.eof()) {
        throw ScoreError("invalid score file");
    }
    std::ranges::sort(scores, std::greater{});
    return scores;
}

void ScoreRepository::save(const std::vector<int>& scores) const {
    std::ofstream output(path_);
    if (!output) {
        throw ScoreError("cannot open score file");
    }
    for (int score : scores) {
        output << score << "\n";
    }
    // flush() catches stream-layer failures; close-time kernel errors may still be deferred.
    if (!output.flush()) {
        throw ScoreError("cannot write score file");
    }
}

void ScoreRepository::addScore(int score, std::size_t limit) const {
    std::vector<int> scores = load();
    scores.push_back(score);
    std::ranges::sort(scores, std::greater{});
    if (scores.size() > limit) {
        scores.resize(limit);
    }
    save(scores);
}
