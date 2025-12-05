#pragma once
#include <string>
#include <map>
#include <memory>
#include <iostream>

struct Solution {
    virtual ~Solution() = default;
    virtual std::string part1(const std::string& input) = 0;
    virtual std::string part2(const std::string& input) = 0;
};

class SolutionRegistry {
public:
    static SolutionRegistry& instance() {
        static SolutionRegistry inst;
        return inst;
    }

    void registerSolution(int day, std::unique_ptr<Solution> sol) {
        solutions[day] = std::move(sol);
    }

    const std::map<int, std::unique_ptr<Solution>>& getAll() const {
        return solutions;
    }

private:
    std::map<int, std::unique_ptr<Solution>> solutions;
};

#define AOC_REGISTER(day_num, class_name)                 \
    namespace {                                           \
        struct Register##day_num {                        \
            Register##day_num() {                         \
                SolutionRegistry::instance().registerSolution(day_num, std::make_unique<class_name>()); \
            }                                             \
        };                                                \
        static Register##day_num reg_##day_num;           \
    }
