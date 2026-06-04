#ifndef TETRIS_ASSIGNMENT_006_SCORE_REPOSITORY_HPP
#define TETRIS_ASSIGNMENT_006_SCORE_REPOSITORY_HPP

#include <stdexcept>
#include <string>
#include <vector>

class ScoreError : public std::runtime_error {
public:
    explicit ScoreError(const std::string& message);
};

class ScoreRepository {
public:
    explicit ScoreRepository(std::string path);

    std::vector<int> load() const;
    void save(const std::vector<int>& scores) const;
    void addScore(int score, std::size_t limit = 5) const;

private:
    std::string path_;
};

#endif
